#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Util/Heap.h"
#include "V2MPInternal/Util/Util.h"
#include "Modules/CPU_Internal.h"

V2MP_CPURenameMe* V2MP_CPURenameMe_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_CPURenameMe);
}

void V2MP_CPURenameMe_DeinitAndFree(V2MP_CPURenameMe* cpu)
{
	if ( cpu )
	{
		V2MP_FREE(cpu);
	}
}

void V2MP_CPURenameMe_SetSupervisorInterface(V2MP_CPURenameMe* cpu, const V2MP_CPURenameMe_SupervisorInterface* interface)
{
	if ( !cpu )
	{
		return;
	}

	if ( interface )
	{
		cpu->supervisorInterface = *interface;
	}
	else
	{
		V2MP_ZERO_STRUCT_PTR(&cpu->supervisorInterface);
	}
}

void V2MP_CPURenameMe_Reset(V2MP_CPURenameMe* cpu)
{
	if ( !cpu )
	{
		return;
	}

	cpu->pc = 0;
	cpu->sr = 0;
	cpu->lr = 0;
	cpu->r0 = 0;
	cpu->r1 = 0;
	cpu->ir = 0;
	cpu->fault = 0;
}

void V2MP_CPURenameMe_NotifyFault(V2MP_CPURenameMe* cpu, V2MP_Fault fault, V2MP_Word args)
{
	if ( !cpu )
	{
		return;
	}

	cpu->fault = V2MP_CPU_MAKE_FAULT_WORD(fault, args);
}

bool V2MP_CPURenameMe_HasFault(V2MP_CPURenameMe* cpu)
{
	return cpu && V2MP_CPU_FAULT_CODE(cpu->fault) != V2MP_FAULT_NONE;
}

V2MP_Word V2MP_CPURenameMe_GetFaultWord(V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->fault : 0;
}
