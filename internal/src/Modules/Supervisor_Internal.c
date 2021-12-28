#include "Modules/Supervisor_Internal.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/Mainboard.h"

void V2MP_Supervisor_SetCPUFault(V2MP_Supervisor* supervisor, V2MP_Word fault)
{
	V2MP_CPU* cpu;

	if ( !supervisor )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPU_NotifyFault(cpu, fault);
	}
}

V2MP_Word V2MP_Supervisor_FetchInstructionWord(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word* destReg)
{
	V2MP_MemoryStore* memoryStore;

	if ( !supervisor )
	{
		return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0);
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0);
	}

	if ( !V2MP_MemoryStore_LoadWord(memoryStore, address, destReg) )
	{
		return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0);
	}

	return V2MP_FAULT_NONE;
}

void V2MP_Supervisor_RequestLoadWordFromDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_RegisterIndex destReg)
{
	V2MP_Supervisor_Action* action;

	action = V2MP_Supervisor_CreateNewAction(supervisor);

	if ( !action )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, SVAT_LOAD_WORD));
		return;
	}

	action->actionType = SVAT_LOAD_WORD;
	SVACTION_LOAD_WORD_ARG_ADDRESS(action) = address;
	SVACTION_LOAD_WORD_ARG_DESTREG(action) = destReg;
}

void V2MP_Supervisor_RequestStoreWordToDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word wordToStore)
{
	V2MP_Supervisor_Action* action;

	action = V2MP_Supervisor_CreateNewAction(supervisor);

	if ( !action )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, SVAT_STORE_WORD));
		return;
	}

	action->actionType = SVAT_STORE_WORD;
	SVACTION_STORE_WORD_ARG_ADDRESS(action) = address;
	SVACTION_STORE_WORD_ARG_WORD(action) = wordToStore;
}
