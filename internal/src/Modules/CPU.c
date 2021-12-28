#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Util/Heap.h"
#include "V2MPInternal/Util/Util.h"
#include "Modules/CPU_Internal.h"
#include "Modules/CPU_Instructions.h"

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

void V2MP_CPURenameMe_ResetSupervisorInterface(V2MP_CPURenameMe* cpu)
{
	V2MP_CPURenameMe_SetSupervisorInterface(cpu, NULL);
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

bool V2MP_CPURenameMe_ExecuteClockCycle(V2MP_CPURenameMe* cpu)
{
	V2MP_Word fault;

	if ( !cpu || !cpu->supervisorInterface.fetchInstructionWord )
	{
		return false;
	}

	fault = cpu->supervisorInterface.fetchInstructionWord(cpu->supervisorInterface.supervisor, cpu->pc, &cpu->ir);

	if ( V2MP_CPU_FAULT_CODE(fault) != V2MP_FAULT_NONE )
	{
		V2MP_CPURenameMe_NotifyFault(cpu, fault);
		return true;
	}

	cpu->pc += 2;

	return V2MP_CPURenameMe_ExecuteInstructionInternal(cpu);
}

void V2MP_CPURenameMe_NotifyFault(V2MP_CPURenameMe* cpu, V2MP_Fault fault)
{
	if ( !cpu )
	{
		return;
	}

	cpu->fault = fault;
}

bool V2MP_CPURenameMe_HasFault(const V2MP_CPURenameMe* cpu)
{
	return cpu && V2MP_CPU_FAULT_CODE(cpu->fault) != V2MP_FAULT_NONE;
}

V2MP_Word V2MP_CPURenameMe_GetFaultWord(const V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->fault : 0;
}

bool V2MP_CPURenameMe_SetRegisterValueAndUpdateSR(V2MP_CPURenameMe* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value)
{
	V2MP_Word* regPtr;

	if ( !cpu )
	{
		return false;
	}

	regPtr = V2MP_CPURenameMe_GetRegisterPtr(cpu, regIndex);

	if ( !regPtr )
	{
		return false;
	}

	*regPtr = value;

	cpu->sr = 0;

	if ( *regPtr == 0 )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}

	return true;
}

bool V2MP_CPURenameMe_GetRegisterValue(const V2MP_CPURenameMe* cpu, V2MP_RegisterIndex regIndex, V2MP_Word* outValue)
{
	const V2MP_Word* regPtr;

	if ( !cpu || !outValue )
	{
		return false;
	}

	regPtr = V2MP_CPURenameMe_GetRegisterConstPtr(cpu, regIndex);

	if ( !regPtr )
	{
		return false;
	}

	*outValue = *regPtr;
	return true;
}

bool V2MP_CPURenameMe_ExecuteSingleInstruction(V2MP_CPURenameMe* cpu, V2MP_Word instruction)
{
	if ( !cpu )
	{
		return false;
	}

	cpu->ir = instruction;

	return V2MP_CPURenameMe_ExecuteInstructionInternal(cpu);
}

V2MP_Word V2MP_CPURenameMe_GetProgramCounter(const V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->pc : 0;
}

void V2MP_CPURenameMe_SetProgramCounter(V2MP_CPURenameMe* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->pc = value;
}

V2MP_Word V2MP_CPURenameMe_GetStatusRegister(const V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->sr : 0;
}

void V2MP_CPURenameMe_SetStatusRegister(V2MP_CPURenameMe* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->sr = value;
}

V2MP_Word V2MP_CPURenameMe_GetLinkRegister(const V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->lr : 0;
}

void V2MP_CPURenameMe_SetLinkRegister(V2MP_CPURenameMe* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->lr = value;
}

V2MP_Word V2MP_CPURenameMe_GetR0(const V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->r0 : 0;
}

void V2MP_CPURenameMe_SetR0(V2MP_CPURenameMe* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->r0 = value;
}

V2MP_Word V2MP_CPURenameMe_GetR1(const V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->r1 : 0;
}

void V2MP_CPURenameMe_SetR1(V2MP_CPURenameMe* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->r1 = value;
}

V2MP_Word V2MP_CPURenameMe_GetInstructionRegister(const V2MP_CPURenameMe* cpu)
{
	return cpu ? cpu->ir : 0;
}

void V2MP_CPURenameMe_SetInstructionRegister(V2MP_CPURenameMe* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->ir = value;
}
