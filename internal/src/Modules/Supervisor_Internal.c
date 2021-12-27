#include "Modules/Supervisor_Internal.h"
#include "V2MPInternal/Modules/CPU.h"

void V2MP_Supervisor_SetCPUFault(V2MP_Supervisor* supervisor, V2MP_Word fault)
{
	V2MP_CPURenameMe* cpu;

	if ( !supervisor )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPURenameMe_NotifyFault(cpu, fault);
	}
}

V2MP_Word V2MP_Supervisor_FetchInstructionWord(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word* destReg)
{
	// TODO
	(void)supervisor;
	(void)address;
	(void)destReg;

	return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0);
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
