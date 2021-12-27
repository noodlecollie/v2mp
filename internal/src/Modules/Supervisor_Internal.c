#include "Modules/Supervisor_Internal.h"
#include "V2MPInternal/Modules/CPU.h"

V2MP_Word V2MP_Supervisor_FetchInstructionWord(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word* destReg)
{
	// TODO
	(void)supervisor;
	(void)address;
	(void)destReg;

	return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0);
}

void V2MP_Supervisor_RequestLoadWordFromDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word* destReg)
{
	V2MP_CPURenameMe* cpu;
	if ( !supervisor )
	{
		return;
	}

	// TODO
	(void)address;
	(void)destReg;
	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);
	V2MP_CPURenameMe_NotifyFault(cpu, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0));
}

void V2MP_Supervisor_RequestStoreWordToDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word wordToStore)
{
	V2MP_CPURenameMe* cpu;
	if ( !supervisor )
	{
		return;
	}

	// TODO
	(void)address;
	(void)wordToStore;
	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);
	V2MP_CPURenameMe_NotifyFault(cpu, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0));
}
