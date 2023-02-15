#include <string.h>
#include "Modules/Supervisor_Action.h"
#include "Modules/Supervisor_Internal.h"
#include "SharedComponents/DoubleLinkedList.h"
#include "SharedComponents/CircularBuffer.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/CPU.h"
#include "LibBaseUtil/Util.h"
#include "Modules/Supervisor_Action_Stack.h"

typedef enum ActionResult
{
	AR_FAILED = 0,
	AR_COMPLETE,
	AR_ONGOING
} ActionResult;

typedef ActionResult (*ActionHandler)(V2MP_Supervisor*, V2MP_Supervisor_Action*);

static ActionResult V2MP_Supervisor_HandleLoadWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleStoreWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);
static ActionResult V2MP_Supervisor_HandleStackOperation(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action);

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

	action = (V2MP_Supervisor_Action*)V2MPSC_DoubleLLNode_GetPayload(node);

	if ( action )
	{
		BASEUTIL_ZERO_STRUCT_PTR(action);
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

	action = (V2MP_Supervisor_Action*)V2MPSC_DoubleLLNode_GetPayload(node);

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

		action = (V2MP_Supervisor_Action*)V2MPSC_DoubleLLNode_GetPayload(node);
		result = ResolveAction(supervisor, action);
		next = V2MPSC_DoubleLLNode_GetNext(node);

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

		V2MPSC_DoubleLLNode_Destroy(node);

		if ( result == AR_FAILED )
		{
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0));
			break;
		}

		node = next;
	}

	if ( lastProcessedOngoingActionNode )
	{
		node = V2MPSC_DoubleLLNode_GetNext(lastProcessedOngoingActionNode);
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

		action = (V2MP_Supervisor_Action*)V2MPSC_DoubleLLNode_GetPayload(node);
		result = ResolveAction(supervisor, action);
		next = V2MPSC_DoubleLLNode_GetNext(node);

		if ( result != AR_ONGOING )
		{
			V2MPSC_DoubleLLNode_Destroy(node);
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
