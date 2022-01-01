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

static ActionResult V2MP_Supervisor_HandleLoadWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleStoreWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleInitDeviceDataTransfer(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);

typedef ActionResult (*ActionHandler)(V2MP_Supervisor*, V2MP_Supervisor_Action*);

#define LIST_ITEM(value, handler) handler,
static const ActionHandler ACTION_HANDLERS[] =
{
	V2MP_SUPERVISOR_ACTION_LIST
};
#undef LIST_ITEM

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

static ActionResult V2MP_Supervisor_HandleInitDeviceDataTransfer(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_DevicePortCollection* ports;
	V2MP_DevicePort* port;
	V2MP_CircularBuffer* mailbox;
	V2MP_CPU* cpu;
	V2MP_Word dsAddress;
	V2MP_Word dsSize;

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu )
	{
		return AR_FAILED;
	}

	ports = V2MP_Mainboard_GetDevicePortCollection(supervisor->mainboard);

	if ( !ports )
	{
		return AR_FAILED;
	}

	port = V2MP_DevicePortCollection_GetPort(ports, SVACTION_INIT_DDT_ARG_PORT(action));

	if ( !port || !V2MP_DevicePort_HasConnectedDevice(port) )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_IDO, 0));
		return AR_COMPLETE;
	}

	mailbox = V2MP_DevicePort_GetMailbox(port);

	if ( !mailbox )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_IDO, 0));
		return AR_COMPLETE;
	}

	if ( V2MP_DevicePort_GetMailboxController(port) != V2MP_MBC_PROGRAM )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_IDO, 0));
		return AR_COMPLETE;
	}

	dsAddress = SVACTION_INIT_DDT_ARG_DS_ADDR(action);
	dsSize = SVACTION_INIT_DDT_ARG_DS_SIZE(action);

	if ( dsSize < 1 )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_IDO, 0));
		return AR_COMPLETE;
	}

	// We will want to do this over multiple clock cycles eventually,
	// depending on the data transfer speed of the device.
	if ( SVACTION_INIT_DDT_ARG_IS_MB_WRITE(action) )
	{
		const V2MP_Byte* dsData;

		dsData = V2MP_Supervisor_GetConstDataRangeFromSegment(supervisor, &supervisor->programDS, dsAddress, dsSize);

		if ( dsData )
		{
			size_t bytesWrittenToMailbox;
			size_t freeBytesInMailbox;
			V2MP_Word sr = 0;

			freeBytesInMailbox = V2MP_CircularBuffer_BytesFree(mailbox);
			bytesWrittenToMailbox = V2MP_CircularBuffer_WriteData(mailbox, dsData, dsSize);

			if ( V2MP_CircularBuffer_IsFull(mailbox) )
			{
				sr |= V2MP_CPU_SR_Z;
			}

			if ( freeBytesInMailbox < dsSize )
			{
				sr |= V2MP_CPU_SR_C;
			}

			V2MP_CPU_SetR1(cpu, (V2MP_Word)bytesWrittenToMailbox);
			V2MP_CPU_SetStatusRegister(cpu, sr);
		}
		else
		{
			// TODO: Determine how many bytes we can write, and write those instead?
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));
		}
	}
	else
	{
		V2MP_Byte* dsData;

		dsData = V2MP_Supervisor_GetDataRangeFromSegment(supervisor, &supervisor->programDS, dsAddress, dsSize);

		if ( dsData )
		{
			size_t bytesReadFromMailbox;
			size_t origBytesInMailbox;
			V2MP_Word sr = 0;

			origBytesInMailbox = V2MP_CircularBuffer_BytesUsed(mailbox);
			bytesReadFromMailbox = V2MP_CircularBuffer_ReadData(mailbox, dsData, dsSize);

			if ( bytesReadFromMailbox < dsSize )
			{
				sr |= V2MP_CPU_SR_C;
			}

			if ( bytesReadFromMailbox >= origBytesInMailbox )
			{
				sr |= V2MP_CPU_SR_Z;
			}

			V2MP_CPU_SetR1(cpu, (V2MP_Word)bytesReadFromMailbox);
			V2MP_CPU_SetStatusRegister(cpu, sr);
		}
		else
		{
			// TODO: Determine how many bytes we can write, and write those instead?
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));
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
			break;
		}

		node = next;
	}

	return true;
}
