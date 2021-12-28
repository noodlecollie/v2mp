#include <string.h>
#include "Modules/Supervisor_Action.h"
#include "Modules/Supervisor_Internal.h"
#include "V2MPInternal/Components/DoubleLinkedList.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/CPU.h"

static bool LoadWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_MemoryStoreRenameMe* memoryStore;
	V2MP_CPU* cpu;
	size_t address;
	V2MP_RegisterIndex destReg;
	V2MP_Word loadedWord = 0;

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return false;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu )
	{
		return false;
	}

	address = supervisor->programDS.base + SVACTION_LOAD_WORD_ARG_ADDRESS(action);
	destReg = (V2MP_RegisterIndex)SVACTION_LOAD_WORD_ARG_DESTREG(action);

	if ( address & 0x1 )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_ALGN, 0));
		return true;
	}

	if ( !V2MP_MemoryStoreRenameMe_LoadWord(memoryStore, address, &loadedWord) )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));
		return true;
	}

	V2MP_CPU_SetRegisterValueAndUpdateSR(cpu, destReg, loadedWord);
	return true;
}

static bool StoreWord(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	V2MP_MemoryStoreRenameMe* memoryStore;
	size_t address;
	V2MP_Word wordToStore;

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return false;
	}

	address = supervisor->programDS.base + SVACTION_STORE_WORD_ARG_ADDRESS(action);
	wordToStore = SVACTION_STORE_WORD_ARG_WORD(action);

	if ( address & 0x1 )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_ALGN, 0));
		return true;
	}

	if ( !V2MP_MemoryStoreRenameMe_StoreWord(memoryStore, address, wordToStore) )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0));
	}

	return true;
}

static bool ResolveAction(V2MP_Supervisor* supervisor, V2MP_Supervisor_Action* action)
{
	if ( !action )
	{
		return false;
	}

	switch ( action->actionType )
	{
		case SVAT_LOAD_WORD:
		{
			return LoadWord(supervisor, action);
		}

		case SVAT_STORE_WORD:
		{
			return StoreWord(supervisor, action);
		}

		default:
		{
			V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, action->actionType));
			return false;
		}
	}
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
		bool success;

		success = ResolveAction(supervisor, (V2MP_Supervisor_Action*)V2MP_DoubleLLNode_GetPayload(node));

		next = V2MP_DoubleLLNode_GetNext(node);
		V2MP_DoubleLLNode_Destroy(node);

		if ( !success )
		{
			break;
		}

		node = next;
	}

	return true;
}
