#include <string.h>
#include "Modules/Supervisor_Action.h"
#include "Modules/Supervisor_Internal.h"
#include "V2MPInternal/Components/DoubleLinkedList.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Util/Util.h"
#include "Modules/DevicePort_Internal.h"

typedef enum ActionResult
{
	AR_FAILED = 0,
	AR_COMPLETE,
	AR_ONGOING
} ActionResult;

typedef enum DataTransferContextResult
{
	DTCR_SUCCESSFUL = 0,
	DTCR_FAILED,
	DTCR_NO_PORT,
	DTCR_NO_DEVICE,
	DTCR_NO_MAILBOX,
	DTCR_INVALID_CONTROLLER,
	DTCR_EMPTY_DS_BUFFER
} DataTransferContextResult;

typedef struct DataTransferContext
{
	V2MP_Supervisor* supervisor;
	V2MP_DevicePort* port;
	V2MP_Device* device;
	V2MP_CircularBuffer* mailbox;
	V2MP_CPU* cpu;
	V2MP_Supervisor_Action* action;
} DataTransferContext;

static ActionResult V2MP_Supervisor_HandleLoadWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleStoreWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleDeviceDataTransfer(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);

typedef ActionResult (*ActionHandler)(V2MP_Supervisor*, V2MP_Supervisor_Action*);

#define LIST_ITEM(value, handler) handler,
static const ActionHandler ACTION_HANDLERS[] =
{
	V2MP_SUPERVISOR_ACTION_LIST
};
#undef LIST_ITEM

static DataTransferContextResult ConstructDataTransferContext(
	V2MP_Supervisor* supervisor,
	V2MP_Supervisor_Action* action,
	DataTransferContext* context)
{
	V2MP_DevicePortCollection* ports;
	V2MP_Word flags;

	if ( !supervisor || !action || !context )
	{
		return DTCR_FAILED;
	}

	V2MP_ZERO_STRUCT_PTR(context);

	context->supervisor = supervisor;
	context->action = action;

	context->cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !context->cpu )
	{
		return DTCR_FAILED;
	}

	ports = V2MP_Mainboard_GetDevicePortCollection(supervisor->mainboard);

	if ( !ports )
	{
		return DTCR_FAILED;
	}

	context->port = V2MP_DevicePortCollection_GetPort(ports, SVACTION_DDT_ARG_PORT(action));

	if ( !context->port )
	{
		return DTCR_NO_PORT;
	}

	context->device = V2MP_DevicePort_GetConnectedDevice(context->port);

	if ( !context->device )
	{
		return DTCR_NO_DEVICE;
	}

	context->mailbox = V2MP_DevicePort_GetMailbox(context->port);

	if ( !context->mailbox )
	{
		return DTCR_NO_MAILBOX;
	}

	flags = SVACTION_DDT_ARG_FLAGS(context->action);

	if ( ((!(flags & SVACTION_DDT_FLAG_IS_IN_PROGRESS) && V2MP_DevicePort_GetMailboxController(context->port) != V2MP_DMBC_PROGRAM)) ||
	     ((flags & SVACTION_DDT_FLAG_IS_IN_PROGRESS) && V2MP_DevicePort_GetMailboxController(context->port) != V2MP_DMBC_SUPERVISOR) )
	{
		return DTCR_INVALID_CONTROLLER;
	}

	if ( SVACTION_DDT_ARG_DS_SIZE(context->action) < 1 )
	{
		return DTCR_EMPTY_DS_BUFFER;
	}

	return DTCR_SUCCESSFUL;
}

static ActionResult HandleDataTransferRead(DataTransferContext* context)
{
	V2MP_Word oldActionFlags;
	size_t bytesToRead;
	V2MP_Byte* dsData;
	size_t origBytesInMailbox;
	size_t bytesReadFromMailbox;
	bool exceededSegment = false;

	oldActionFlags = SVACTION_DDT_ARG_FLAGS(context->action);
	bytesToRead = V2MP_Device_GetDataTransferSpeed(context->device);

	if ( bytesToRead < 1 || bytesToRead > SVACTION_DDT_ARG_DS_SIZE(context->action) )
	{
		bytesToRead = SVACTION_DDT_ARG_DS_SIZE(context->action);
	}

	dsData = V2MP_Supervisor_GetDataRangeFromSegment(
		context->supervisor,
		&context->supervisor->programDS,
		SVACTION_DDT_ARG_DS_ADDR(context->action),
		bytesToRead
	);

	if ( !dsData )
	{
		exceededSegment = true;

		bytesToRead = V2MP_Supervisor_GetMaxDSBytesAvailableAtAddress(
			context->supervisor,
			SVACTION_DDT_ARG_DS_ADDR(context->action)
		);

		dsData = V2MP_Supervisor_GetDataRangeFromSegment(
			context->supervisor,
			&context->supervisor->programDS,
			SVACTION_DDT_ARG_DS_ADDR(context->action),
			bytesToRead
		);

		// Should never happen:
		if ( !dsData )
		{
			return AR_FAILED;
		}
	}

	V2MP_DevicePort_PassMailboxControlToSupervisor(context->port);

	origBytesInMailbox = V2MP_CircularBuffer_BytesUsed(context->mailbox);
	bytesReadFromMailbox = V2MP_CircularBuffer_ReadData(context->mailbox, dsData, bytesToRead);

	if ( exceededSegment )
	{
		V2MP_Supervisor_SetCPUFault(context->supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));

		SVACTION_DDT_ARG_FLAGS(context->action) &= ~SVACTION_DDT_FLAG_IS_IN_PROGRESS;
	}
	else
	{
		// If this was the first transfer, update the CPU Registers.
		if ( !(oldActionFlags & SVACTION_DDT_FLAG_IS_IN_PROGRESS) )
		{
			V2MP_Word sr = 0;

			if ( origBytesInMailbox < SVACTION_DDT_ARG_DS_SIZE(context->action) )
			{
				sr |= V2MP_CPU_SR_C;
			}

			if ( SVACTION_DDT_ARG_DS_SIZE(context->action) >= origBytesInMailbox )
			{
				sr |= V2MP_CPU_SR_Z;
			}

			V2MP_CPU_SetR1(context->cpu, (V2MP_Word)V2MP_MIN(origBytesInMailbox, SVACTION_DDT_ARG_DS_SIZE(context->action)));
			V2MP_CPU_SetStatusRegister(context->cpu, sr);
		}

		SVACTION_DDT_ARG_DS_ADDR(context->action) += (V2MP_Word)bytesReadFromMailbox;
		SVACTION_DDT_ARG_DS_SIZE(context->action) -= (V2MP_Word)bytesReadFromMailbox;

		if ( SVACTION_DDT_ARG_DS_SIZE(context->action) < 1 || V2MP_CircularBuffer_IsEmpty(context->mailbox) )
		{
			SVACTION_DDT_ARG_FLAGS(context->action) &= ~SVACTION_DDT_FLAG_IS_IN_PROGRESS;
		}
		else
		{
			SVACTION_DDT_ARG_FLAGS(context->action) |= SVACTION_DDT_FLAG_IS_IN_PROGRESS;
		}
	}

	if ( SVACTION_DDT_ARG_FLAGS(context->action) & SVACTION_DDT_FLAG_IS_IN_PROGRESS )
	{
		return AR_ONGOING;
	}
	else
	{
		V2MP_DevicePort_PassMailboxControlToProgram(context->port);
		return AR_COMPLETE;
	}
}

static ActionResult HandleDataTransferWrite(DataTransferContext* context)
{
	V2MP_Word oldActionFlags;
	size_t bytesToWrite;
	const V2MP_Byte* dsData;
	size_t origBytesFreeInMailbox;
	size_t bytesWrittenToMailbox;
	bool exceededSegment = false;

	oldActionFlags = SVACTION_DDT_ARG_FLAGS(context->action);
	bytesToWrite = V2MP_Device_GetDataTransferSpeed(context->device);

	if ( bytesToWrite < 1 || bytesToWrite > SVACTION_DDT_ARG_DS_SIZE(context->action) )
	{
		bytesToWrite = SVACTION_DDT_ARG_DS_SIZE(context->action);
	}

	dsData = V2MP_Supervisor_GetConstDataRangeFromSegment(
		context->supervisor,
		&context->supervisor->programDS,
		SVACTION_DDT_ARG_DS_ADDR(context->action),
		bytesToWrite
	);

	if ( !dsData )
	{
		exceededSegment = true;

		bytesToWrite = V2MP_Supervisor_GetMaxDSBytesAvailableAtAddress(
			context->supervisor,
			SVACTION_DDT_ARG_DS_ADDR(context->action)
		);

		dsData = V2MP_Supervisor_GetConstDataRangeFromSegment(
			context->supervisor,
			&context->supervisor->programDS,
			SVACTION_DDT_ARG_DS_ADDR(context->action),
			bytesToWrite
		);

		// Should never happen:
		if ( !dsData )
		{
			return AR_FAILED;
		}
	}

	origBytesFreeInMailbox = V2MP_CircularBuffer_BytesFree(context->mailbox);
	bytesWrittenToMailbox = V2MP_CircularBuffer_WriteData(context->mailbox, dsData, bytesToWrite);

	if ( exceededSegment )
	{
		V2MP_Supervisor_SetCPUFault(context->supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));

		SVACTION_DDT_ARG_DS_SIZE(context->action) = 0;
		SVACTION_DDT_ARG_FLAGS(context->action) &= ~SVACTION_DDT_FLAG_IS_IN_PROGRESS;
	}
	else
	{
		// If this was the first transfer, update the CPU Registers.
		if ( !(oldActionFlags & SVACTION_DDT_FLAG_IS_IN_PROGRESS) )
		{
			V2MP_Word sr = 0;

			if ( origBytesFreeInMailbox < SVACTION_DDT_ARG_DS_SIZE(context->action) )
			{
				sr |= V2MP_CPU_SR_C;
			}

			if ( origBytesFreeInMailbox <= SVACTION_DDT_ARG_DS_SIZE(context->action) )
			{
				sr |= V2MP_CPU_SR_Z;
			}

			V2MP_CPU_SetR1(context->cpu, (V2MP_Word)V2MP_MIN(origBytesFreeInMailbox, SVACTION_DDT_ARG_DS_SIZE(context->action)));
			V2MP_CPU_SetStatusRegister(context->cpu, sr);
		}

		SVACTION_DDT_ARG_DS_ADDR(context->action) += (V2MP_Word)bytesWrittenToMailbox;
		SVACTION_DDT_ARG_DS_SIZE(context->action) -= (V2MP_Word)bytesWrittenToMailbox;

		if ( SVACTION_DDT_ARG_DS_SIZE(context->action) < 1 || V2MP_CircularBuffer_IsFull(context->mailbox) )
		{
			SVACTION_DDT_ARG_FLAGS(context->action) &= ~SVACTION_DDT_FLAG_IS_IN_PROGRESS;
		}
		else
		{
			SVACTION_DDT_ARG_FLAGS(context->action) |= SVACTION_DDT_FLAG_IS_IN_PROGRESS;
		}
	}

	return (SVACTION_DDT_ARG_FLAGS(context->action) & SVACTION_DDT_FLAG_IS_IN_PROGRESS)
		? AR_ONGOING
		: AR_COMPLETE;
}

static ActionResult V2MP_Supervisor_HandleLoadWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_MemoryStore* memoryStore;
	V2MP_CPU* cpu;
	size_t address;
	V2MP_RegisterIndex destReg;
	V2MP_Word loadedWord = 0;

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return AR_FAILED;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu )
	{
		return AR_FAILED;
	}

	address = supervisor->programDS.base + SVACTION_LOAD_WORD_ARG_ADDRESS(action);
	destReg = (V2MP_RegisterIndex)SVACTION_LOAD_WORD_ARG_DESTREG(action);

	if ( address & 0x1 )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_ALGN, 0));
		return AR_COMPLETE;
	}

	if ( !V2MP_MemoryStore_LoadWord(memoryStore, address, &loadedWord) )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));
		return AR_COMPLETE;
	}

	V2MP_CPU_SetRegisterValueAndUpdateSR(cpu, destReg, loadedWord);
	return AR_COMPLETE;
}

static ActionResult V2MP_Supervisor_HandleStoreWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_MemoryStore* memoryStore;
	size_t address;
	V2MP_Word wordToStore;

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return AR_FAILED;
	}

	address = supervisor->programDS.base + SVACTION_STORE_WORD_ARG_ADDRESS(action);
	wordToStore = SVACTION_STORE_WORD_ARG_WORD(action);

	if ( address & 0x1 )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_ALGN, 0));
		return AR_COMPLETE;
	}

	if ( !V2MP_MemoryStore_StoreWord(memoryStore, address, wordToStore) )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));
	}

	return AR_COMPLETE;
}

static ActionResult V2MP_Supervisor_HandleDeviceDataTransfer(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	DataTransferContext context;
	DataTransferContextResult contextResult;

	contextResult = ConstructDataTransferContext(supervisor, action, &context);

	switch ( contextResult )
	{
		case DTCR_SUCCESSFUL:
		{
			// All fine, so continue.
			break;
		}

		case DTCR_FAILED:
		{
			// Something was fundementally wrong.
			return AR_FAILED;
		}

		default:
		{
			// Some other error which should result in a fault.
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_IDO, 0));
			return AR_COMPLETE;
		}
	}

	if ( SVACTION_DDT_ARG_FLAGS(action) & SVACTION_DDT_FLAG_IS_MB_WRITE )
	{
		return HandleDataTransferWrite(&context);
	}
	else
	{
		return HandleDataTransferRead(&context);
	}
}

static ActionResult ResolveAction(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	if ( !action )
	{
		return AR_FAILED;
	}

	if ( (size_t)action->actionType >= V2MP_ARRAY_SIZE(ACTION_HANDLERS) ||
	     !ACTION_HANDLERS[(size_t)action->actionType] )
	{
		return AR_FAILED;
	}

	return ACTION_HANDLERS[(size_t)action->actionType](supervisor, action);
}

bool V2MP_Supervisor_CreateActionList(V2MP_Supervisor* supervisor)
{
	if ( !supervisor )
	{
		return false;
	}

	supervisor->actions = V2MP_DoubleLL_AllocateAndInit(sizeof(V2MP_Supervisor_Action), NULL);
	return supervisor->actions != NULL;
}

void V2MP_Supervisor_DestroyActionList(V2MP_Supervisor* supervisor)
{
	if ( !supervisor )
	{
		return;
	}

	V2MP_DoubleLL_DeinitAndFree(supervisor->actions);
}

V2MP_Supervisor_Action* V2MP_Supervisor_CreateNewAction(V2MP_Supervisor* supervisor)
{
	V2MP_DoubleLL_Node* node;
	V2MP_Supervisor_Action* action;

	if ( !supervisor )
	{
		return NULL;
	}

	node = V2MP_DoubleLL_AppendToTail(supervisor->actions);

	if ( !node )
	{
		return NULL;
	}

	action = (V2MP_Supervisor_Action*)V2MP_DoubleLLNode_GetPayload(node);

	if ( action )
	{
		memset(action, 0, sizeof(*action));
	}

	return action;
}

bool V2MP_Supervisor_ResolveOutstandingActions(V2MP_Supervisor* supervisor)
{
	V2MP_DoubleLL_Node* node;

	if ( !supervisor )
	{
		return false;
	}

	node = V2MP_DoubleLL_GetHead(supervisor->actions);

	while ( node )
	{
		V2MP_DoubleLL_Node* next;
		ActionResult result;

		result = ResolveAction(supervisor, (V2MP_Supervisor_Action*)V2MP_DoubleLLNode_GetPayload(node));

		next = V2MP_DoubleLLNode_GetNext(node);

		if ( result != AR_ONGOING )
		{
			V2MP_DoubleLLNode_Destroy(node);
		}

		if ( result == AR_FAILED )
		{
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0));
			break;
		}

		node = next;
	}

	return true;
}
