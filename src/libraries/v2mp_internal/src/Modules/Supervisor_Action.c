#include <string.h>
#include "Modules/Supervisor_Action.h"
#include "Modules/Supervisor_Internal.h"
#include "SharedComponents/DoubleLinkedList.h"
#include "SharedComponents/CircularBuffer.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Modules/CPU.h"
#include "BaseUtil/Util.h"
#include "Modules/DevicePort_Internal.h"
#include "Modules/Supervisor_Action_DPQ.h"
#include "Modules/Supervisor_Action_Stack.h"

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
	DTCR_EMPTY_DS_BUFFER,
	DTCR_INCORRECT_MAILBOX_STATE
} DataTransferContextResult;

typedef struct DataTransferContext
{
	V2MP_Supervisor* supervisor;
	V2MP_DevicePort* port;
	V2MP_Device* device;
	V2MPSC_CircularBuffer* mailbox;
	V2MP_CPU* cpu;
	V2MP_Supervisor_Action* action;
} DataTransferContext;

typedef ActionResult (*ActionHandler)(V2MP_Supervisor*, V2MP_Supervisor_Action*);

static ActionResult V2MP_Supervisor_HandleLoadWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleStoreWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandlePerformDevicePortQuery(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleDeviceDataTransfer(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleRelinquishPortMailbox(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleGetUsableByteCount(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleStackOperation(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);

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

	if ( !supervisor || !action || !context )
	{
		return DTCR_FAILED;
	}

	BASEUTIL_ZERO_STRUCT_PTR(context);

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

	context->port = V2MP_DevicePortCollection_GetPort(ports, SVACTION_DEVDT_ARG_PORT(action));

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

	if ( (SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_INDIRECT) &&
	     SVACTION_DEVDT_ARG_DS_SIZE(context->action) < 1 )
	{
		return DTCR_EMPTY_DS_BUFFER;
	}

	if ( (SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_MB_WRITE) &&
	     !(SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_IN_PROGRESS) &&
	     V2MP_DevicePort_GetMailboxState(context->port) != V2MP_DPMS_WRITEABLE )
	{
		return DTCR_INCORRECT_MAILBOX_STATE;
	}

	if ( !(SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_MB_WRITE) &&
	     !(SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_IN_PROGRESS) &&
	     V2MP_DevicePort_GetMailboxState(context->port) != V2MP_DPMS_READABLE )
	{
		return DTCR_INCORRECT_MAILBOX_STATE;
	}

	return DTCR_SUCCESSFUL;
}

static ActionResult HandleDirectDataTransferRead(DataTransferContext* context)
{
	size_t origBytesInMailbox;
	size_t bytesRead;
	V2MP_Word destWord = 0;
	V2MP_Word sr = 0;

	origBytesInMailbox = V2MPSC_CircularBuffer_BytesUsed(context->mailbox);
	bytesRead = V2MPSC_CircularBuffer_ReadData(context->mailbox, (uint8_t*)&destWord, sizeof(destWord));

	if ( bytesRead < 1 || bytesRead > 2 )
	{
		// Should never happen.
		return AR_FAILED;
	}

	V2MP_CPU_SetLinkRegister(context->cpu, destWord);
	V2MP_CPU_SetR1(context->cpu, (V2MP_Word)bytesRead);

	if ( origBytesInMailbox >= 2 )
	{
		sr |= V2MP_CPU_SR_C;
	}

	if ( V2MPSC_CircularBuffer_IsEmpty(context->mailbox) )
	{
		sr |= V2MP_CPU_SR_Z;
	}

	V2MP_CPU_SetStatusRegister(context->cpu, sr);

	return AR_COMPLETE;
}

static ActionResult HandleIndirectDataTransferRead(DataTransferContext* context)
{
	V2MP_Word oldActionFlags;
	size_t bytesToRead;
	V2MP_Byte* dsData;
	size_t origBytesInMailbox;
	size_t bytesReadFromMailbox;
	bool exceededSegment = false;

	oldActionFlags = SVACTION_DEVDT_ARG_FLAGS(context->action);
	bytesToRead = V2MP_Device_GetDataTransferSpeed(context->device);

	if ( bytesToRead < 1 || bytesToRead > SVACTION_DEVDT_ARG_DS_SIZE(context->action) )
	{
		bytesToRead = SVACTION_DEVDT_ARG_DS_SIZE(context->action);
	}

	dsData = V2MP_Supervisor_GetDataRangeFromSegment(
		context->supervisor,
		&context->supervisor->programDS,
		SVACTION_DEVDT_ARG_DS_ADDR(context->action),
		bytesToRead
	);

	if ( !dsData )
	{
		exceededSegment = true;

		bytesToRead = V2MP_Supervisor_GetMaxDSBytesAvailableAtAddress(
			context->supervisor,
			SVACTION_DEVDT_ARG_DS_ADDR(context->action)
		);

		dsData = V2MP_Supervisor_GetDataRangeFromSegment(
			context->supervisor,
			&context->supervisor->programDS,
			SVACTION_DEVDT_ARG_DS_ADDR(context->action),
			bytesToRead
		);

		// Should never happen:
		if ( !dsData )
		{
			return AR_FAILED;
		}
	}

	V2MP_DevicePort_SetMailboxBusy(context->port, true);

	origBytesInMailbox = V2MPSC_CircularBuffer_BytesUsed(context->mailbox);
	bytesReadFromMailbox = V2MPSC_CircularBuffer_ReadData(context->mailbox, dsData, bytesToRead);

	if ( exceededSegment )
	{
		V2MP_Supervisor_SetCPUFault(context->supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));

		SVACTION_DEVDT_ARG_FLAGS(context->action) &= ~SVACTION_DEVDT_FLAG_IS_IN_PROGRESS;
	}
	else
	{
		// If this was the first transfer, update the CPU Registers.
		if ( !(oldActionFlags & SVACTION_DEVDT_FLAG_IS_IN_PROGRESS) )
		{
			V2MP_Word sr = 0;

			if ( origBytesInMailbox < SVACTION_DEVDT_ARG_DS_SIZE(context->action) )
			{
				sr |= V2MP_CPU_SR_C;
			}

			if ( SVACTION_DEVDT_ARG_DS_SIZE(context->action) >= origBytesInMailbox )
			{
				sr |= V2MP_CPU_SR_Z;
			}

			V2MP_CPU_SetR1(context->cpu, (V2MP_Word)BASEUTIL_MIN(origBytesInMailbox, SVACTION_DEVDT_ARG_DS_SIZE(context->action)));
			V2MP_CPU_SetStatusRegister(context->cpu, sr);
		}

		SVACTION_DEVDT_ARG_DS_ADDR(context->action) += (V2MP_Word)bytesReadFromMailbox;
		SVACTION_DEVDT_ARG_DS_SIZE(context->action) -= (V2MP_Word)bytesReadFromMailbox;

		if ( SVACTION_DEVDT_ARG_DS_SIZE(context->action) < 1 || V2MPSC_CircularBuffer_IsEmpty(context->mailbox) )
		{
			SVACTION_DEVDT_ARG_FLAGS(context->action) &= ~SVACTION_DEVDT_FLAG_IS_IN_PROGRESS;
		}
		else
		{
			SVACTION_DEVDT_ARG_FLAGS(context->action) |= SVACTION_DEVDT_FLAG_IS_IN_PROGRESS;
		}
	}

	if ( SVACTION_DEVDT_ARG_FLAGS(context->action) & SVACTION_DEVDT_FLAG_IS_IN_PROGRESS )
	{
		return AR_ONGOING;
	}
	else
	{
		V2MP_DevicePort_SetMailboxBusy(context->port, false);

		if ( V2MP_DevicePort_GetMailboxController(context->port) == V2MP_DPMC_DEVICE )
		{
			V2MP_DevicePort_NotifyMailboxReadyForInteraction(context->port);
		}

		return AR_COMPLETE;
	}
}

static ActionResult HandleDirectDataTransferWrite(DataTransferContext* context)
{
	size_t origBytesFree;
	size_t bytesWritten;
	V2MP_Word srcWord;
	V2MP_Word sr = 0;

	srcWord = V2MP_CPU_GetLinkRegister(context->cpu);
	origBytesFree = V2MPSC_CircularBuffer_BytesFree(context->mailbox);
	bytesWritten = V2MPSC_CircularBuffer_WriteData(context->mailbox, (const uint8_t*)&srcWord, sizeof(srcWord));

	if ( bytesWritten < 1 || bytesWritten > 2 )
	{
		// Should never happen.
		return AR_FAILED;
	}

	V2MP_CPU_SetR1(context->cpu, (V2MP_Word)bytesWritten);

	if ( V2MPSC_CircularBuffer_IsFull(context->mailbox) )
	{
		sr |= V2MP_CPU_SR_Z;
	}

	if ( origBytesFree < 2 )
	{
		sr |= V2MP_CPU_SR_C;
	}

	V2MP_CPU_SetStatusRegister(context->cpu, sr);

	return AR_COMPLETE;
}

static ActionResult HandleIndirectDataTransferWrite(DataTransferContext* context)
{
	V2MP_Word oldActionFlags;
	size_t bytesToWrite;
	const V2MP_Byte* dsData;
	size_t origBytesFreeInMailbox;
	size_t bytesWrittenToMailbox;
	bool exceededSegment = false;

	oldActionFlags = SVACTION_DEVDT_ARG_FLAGS(context->action);
	bytesToWrite = V2MP_Device_GetDataTransferSpeed(context->device);

	if ( bytesToWrite < 1 || bytesToWrite > SVACTION_DEVDT_ARG_DS_SIZE(context->action) )
	{
		bytesToWrite = SVACTION_DEVDT_ARG_DS_SIZE(context->action);
	}

	dsData = V2MP_Supervisor_GetConstDataRangeFromSegment(
		context->supervisor,
		&context->supervisor->programDS,
		SVACTION_DEVDT_ARG_DS_ADDR(context->action),
		bytesToWrite
	);

	if ( !dsData )
	{
		exceededSegment = true;

		bytesToWrite = V2MP_Supervisor_GetMaxDSBytesAvailableAtAddress(
			context->supervisor,
			SVACTION_DEVDT_ARG_DS_ADDR(context->action)
		);

		dsData = V2MP_Supervisor_GetConstDataRangeFromSegment(
			context->supervisor,
			&context->supervisor->programDS,
			SVACTION_DEVDT_ARG_DS_ADDR(context->action),
			bytesToWrite
		);

		// Should never happen:
		if ( !dsData )
		{
			return AR_FAILED;
		}
	}

	V2MP_DevicePort_SetMailboxBusy(context->port, true);

	origBytesFreeInMailbox = V2MPSC_CircularBuffer_BytesFree(context->mailbox);
	bytesWrittenToMailbox = V2MPSC_CircularBuffer_WriteData(context->mailbox, dsData, bytesToWrite);

	if ( exceededSegment )
	{
		V2MP_Supervisor_SetCPUFault(context->supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));

		SVACTION_DEVDT_ARG_FLAGS(context->action) &= ~SVACTION_DEVDT_FLAG_IS_IN_PROGRESS;
	}
	else
	{
		// If this was the first transfer, update the CPU Registers.
		if ( !(oldActionFlags & SVACTION_DEVDT_FLAG_IS_IN_PROGRESS) )
		{
			V2MP_Word sr = 0;

			if ( origBytesFreeInMailbox < SVACTION_DEVDT_ARG_DS_SIZE(context->action) )
			{
				sr |= V2MP_CPU_SR_C;
			}

			if ( origBytesFreeInMailbox <= SVACTION_DEVDT_ARG_DS_SIZE(context->action) )
			{
				sr |= V2MP_CPU_SR_Z;
			}

			V2MP_CPU_SetR1(context->cpu, (V2MP_Word)BASEUTIL_MIN(origBytesFreeInMailbox, SVACTION_DEVDT_ARG_DS_SIZE(context->action)));
			V2MP_CPU_SetStatusRegister(context->cpu, sr);
		}

		SVACTION_DEVDT_ARG_DS_ADDR(context->action) += (V2MP_Word)bytesWrittenToMailbox;
		SVACTION_DEVDT_ARG_DS_SIZE(context->action) -= (V2MP_Word)bytesWrittenToMailbox;

		if ( SVACTION_DEVDT_ARG_DS_SIZE(context->action) < 1 || V2MPSC_CircularBuffer_IsFull(context->mailbox) )
		{
			SVACTION_DEVDT_ARG_FLAGS(context->action) &= ~SVACTION_DEVDT_FLAG_IS_IN_PROGRESS;
		}
		else
		{
			SVACTION_DEVDT_ARG_FLAGS(context->action) |= SVACTION_DEVDT_FLAG_IS_IN_PROGRESS;
		}
	}

	if ( SVACTION_DEVDT_ARG_FLAGS(context->action) & SVACTION_DEVDT_FLAG_IS_IN_PROGRESS )
	{
		return AR_ONGOING;
	}
	else
	{
		V2MP_DevicePort_SetMailboxBusy(context->port, false);

		if ( V2MP_DevicePort_GetMailboxController(context->port) == V2MP_DPMC_DEVICE )
		{
			V2MP_DevicePort_NotifyMailboxReadyForInteraction(context->port);
		}

		return AR_COMPLETE;
	}
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

static ActionResult V2MP_Supervisor_HandlePerformDevicePortQuery(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	bool result;

	result = V2MP_Supervisor_HandlePerformDevicePortQueryImpl(
		supervisor,
		SVACTION_DPQ_ARG_PORT(action),
		(V2MP_DevicePortQueryType)SVACTION_DPQ_ARG_QUERY_TYPE(action)
	);

	return result ? AR_COMPLETE : AR_FAILED;
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

	if ( SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_INDIRECT )
	{
		if ( SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_MB_WRITE )
		{
			return HandleIndirectDataTransferWrite(&context);
		}
		else
		{
			return HandleIndirectDataTransferRead(&context);
		}
	}
	else
	{
		if ( SVACTION_DEVDT_ARG_FLAGS(action) & SVACTION_DEVDT_FLAG_IS_MB_WRITE )
		{
			return HandleDirectDataTransferWrite(&context);
		}
		else
		{
			return HandleDirectDataTransferRead(&context);
		}
	}
}

static ActionResult V2MP_Supervisor_HandleRelinquishPortMailbox(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_DevicePortCollection* dpc;
	V2MP_DevicePort* port;

	dpc = V2MP_Mainboard_GetDevicePortCollection(supervisor->mainboard);

	if ( !dpc )
	{
		return AR_FAILED;
	}

	port = V2MP_DevicePortCollection_GetPort(dpc, SVACTION_RELINQUISH_MAILBOX_ARG_PORT(action));

	if ( !port )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_IDO, 0));
		return AR_COMPLETE;
	}

	if ( V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM )
	{
		V2MP_DevicePort_ProgramRelinquishMailbox(port);

		if ( !V2MP_DevicePort_IsMailboxBusy(port) )
		{
			V2MP_DevicePort_NotifyMailboxReadyForInteraction(port);
		}
	}

	return AR_COMPLETE;
}

static ActionResult V2MP_Supervisor_HandleGetUsableByteCount(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_DevicePortCollection* dpc;
	V2MP_DevicePort* port;
	V2MP_CPU* cpu;
	V2MP_Word count = 0;

	dpc = V2MP_Mainboard_GetDevicePortCollection(supervisor->mainboard);
	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !dpc || !cpu )
	{
		return AR_FAILED;
	}

	port = V2MP_DevicePortCollection_GetPort(dpc, SVACTION_RELINQUISH_MAILBOX_ARG_PORT(action));

	if ( port )
	{
		V2MPSC_CircularBuffer* mailbox = V2MP_DevicePort_GetMailbox(port);

		if ( mailbox )
		{
			if ( V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE )
			{
				count = (V2MP_Word)V2MPSC_CircularBuffer_BytesUsed(mailbox);
			}
			else if ( V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE )
			{
				count = (V2MP_Word)V2MPSC_CircularBuffer_BytesFree(mailbox);
			}
		}
	}

	V2MP_CPU_SetLinkRegister(cpu, count);
	V2MP_CPU_SetStatusRegister(cpu, count == 0 ? V2MP_CPU_SR_Z : 0);

	return AR_COMPLETE;
}

static ActionResult V2MP_Supervisor_HandleStackOperation(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_CPU* cpu;

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu )
	{
		return AR_FAILED;
	}

	if ( SVACTION_STACK_IS_PUSH(action) )
	{
		V2MP_Word regData[V2MP_REGID_MAX + 1];
		size_t numWords = 0;
		size_t index;

		for ( index = 0; index <= V2MP_REGID_MAX; ++index )
		{
			if ( SVACTION_STACK_REG_FLAGS(action) & (1 << index) )
			{
				if ( !V2MP_CPU_GetRegisterValue(cpu, (V2MP_RegisterIndex)index, &regData[numWords++]) )
				{
					return AR_FAILED;
				}
			}
		}

		if ( !V2MP_Supervisor_PerformStackPush(supervisor, regData, numWords) )
		{
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SOF, 0));
		}
	}
	else
	{
		V2MP_Word regData[V2MP_REGID_MAX + 1];
		size_t numWords = 0;
		size_t index;

		for ( index = 0; index <= V2MP_REGID_MAX; ++index )
		{
			if ( SVACTION_STACK_REG_FLAGS(action) & (1 << index) )
			{
				++numWords;
			}
		}

		if ( V2MP_Supervisor_PerformStackPop(supervisor, regData, numWords) )
		{
			numWords = 0;

			for ( index = 0; index <= V2MP_REGID_MAX; ++index )
			{
				if ( SVACTION_STACK_REG_FLAGS(action) & (1 << index) )
				{
					V2MP_CPU_SetRegisterValue(cpu, (V2MP_RegisterIndex)index, regData[numWords++]);
				}
			}
		}
		else
		{
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SOF, 0));
		}
	}

	return AR_COMPLETE;
}

static ActionResult ResolveAction(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	if ( !action )
	{
		return AR_FAILED;
	}

	if ( (size_t)action->actionType >= BASEUTIL_ARRAY_SIZE(ACTION_HANDLERS) ||
	     !ACTION_HANDLERS[(size_t)action->actionType] )
	{
		return AR_FAILED;
	}

	return ACTION_HANDLERS[(size_t)action->actionType](supervisor, action);
}

bool V2MP_Supervisor_CreateActionLists(V2MP_Supervisor* supervisor)
{
	if ( !supervisor )
	{
		return false;
	}

	supervisor->newActions = V2MPSC_DoubleLL_AllocateAndInit(sizeof(V2MP_Supervisor_Action), NULL);
	supervisor->ongoingActions = V2MPSC_DoubleLL_AllocateAndInit(sizeof(V2MP_Supervisor_Action), NULL);

	return supervisor->newActions != NULL && supervisor->ongoingActions != NULL;
}

void V2MP_Supervisor_DestroyActionLists(V2MP_Supervisor* supervisor)
{
	if ( !supervisor )
	{
		return;
	}

	V2MPSC_DoubleLL_DeinitAndFree(supervisor->newActions);
	supervisor->newActions = NULL;

	V2MPSC_DoubleLL_DeinitAndFree(supervisor->ongoingActions);
	supervisor->ongoingActions = NULL;
}

V2MP_Supervisor_Action* V2MP_Supervisor_CreateNewAction(V2MP_Supervisor* supervisor)
{
	V2MPSC_DoubleLL_Node* node;
	V2MP_Supervisor_Action* action;

	if ( !supervisor )
	{
		return NULL;
	}

	node = V2MPSC_DoubleLL_AppendToTail(supervisor->newActions);

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

V2MPSC_DoubleLL_Node* V2MP_Supervisor_CloneToOngoingAction(V2MP_Supervisor* supervisor, V2MPSC_DoubleLL_Node* createAfter, V2MP_Supervisor_Action* template)
{
	V2MPSC_DoubleLL_Node* node;
	V2MP_Supervisor_Action* action;

	if ( !supervisor || !template )
	{
		return NULL;
	}

	if ( createAfter )
	{
		node = V2MPSC_DoubleLL_CreateAfter(supervisor->ongoingActions, createAfter);
	}
	else
	{
		node = V2MPSC_DoubleLL_PrependToHead(supervisor->ongoingActions);
	}

	if ( !node )
	{
		return NULL;
	}

	action = (V2MP_Supervisor_Action*)V2MP_DoubleLLNode_GetPayload(node);

	if ( action )
	{
		*action = *template;
	}

	return node;
}

bool V2MP_Supervisor_ResolveOutstandingActions(V2MP_Supervisor* supervisor)
{
	V2MPSC_DoubleLL_Node* node;
	V2MPSC_DoubleLL_Node* lastProcessedOngoingActionNode = NULL;

	if ( !supervisor )
	{
		return false;
	}

	node = V2MPSC_DoubleLL_GetHead(supervisor->newActions);

	while ( node )
	{
		V2MPSC_DoubleLL_Node* next;
		ActionResult result;
		V2MP_Supervisor_Action* action;

		action = (V2MP_Supervisor_Action*)V2MP_DoubleLLNode_GetPayload(node);
		result = ResolveAction(supervisor, action);
		next = V2MP_DoubleLLNode_GetNext(node);

		if ( result == AR_ONGOING )
		{
			// Insert at the beginning of the ongoing actions list,
			// and then keep track of the node that was just inserted so
			// that we may inser others after it if we need to.
			// The ongoing actions list is kept separate and processed
			// after this. This is so that new actions created on this
			// clock cycle are processed first, and if they take more
			// than one clock cycle then they are added to the ongoing
			// list. After this, all remaining ongoing actions are
			// processed.

			lastProcessedOngoingActionNode = V2MP_Supervisor_CloneToOngoingAction(supervisor, lastProcessedOngoingActionNode, action);

		}

		V2MP_DoubleLLNode_Destroy(node);

		if ( result == AR_FAILED )
		{
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0));
			break;
		}

		node = next;
	}

	if ( lastProcessedOngoingActionNode )
	{
		node = V2MP_DoubleLLNode_GetNext(lastProcessedOngoingActionNode);
	}
	else
	{
		node = V2MPSC_DoubleLL_GetHead(supervisor->ongoingActions);
	}

	while ( node )
	{
		V2MPSC_DoubleLL_Node* next;
		ActionResult result;
		V2MP_Supervisor_Action* action;

		action = (V2MP_Supervisor_Action*)V2MP_DoubleLLNode_GetPayload(node);
		result = ResolveAction(supervisor, action);
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
