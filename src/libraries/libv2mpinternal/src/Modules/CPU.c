#include "LibV2MPInternal/Modules/CPU.h"
#include "LibBaseUtil/Heap.h"
#include "LibBaseUtil/Util.h"
#include "Modules/CPU_Internal.h"
#include "Modules/CPU_Instructions.h"

V2MP_CPU* V2MP_CPU_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MP_CPU);
}

void V2MP_CPU_DeinitAndFree(V2MP_CPU* cpu)
{
	if ( cpu )
	{
		BASEUTIL_FREE(cpu);
	}
}

void V2MP_CPU_SetSupervisorInterface(V2MP_CPU* cpu, const V2MP_CPU_SupervisorInterface* interface)
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
		BASEUTIL_ZERO_STRUCT_PTR(&cpu->supervisorInterface);
	}
}

void V2MP_CPU_ResetSupervisorInterface(V2MP_CPU* cpu)
{
	V2MP_CPU_SetSupervisorInterface(cpu, NULL);
}

void V2MP_CPU_Reset(V2MP_CPU* cpu)
{
	if ( !cpu )
	{
		return;
	}

	cpu->pc = 0;
	cpu->sp = 0;
	cpu->sr = 0;
	cpu->lr = 0;
	cpu->r0 = 0;
	cpu->r1 = 0;
	cpu->ir = 0;
	cpu->fault = 0;
}

bool V2MP_CPU_ExecuteClockCycle(V2MP_CPU* cpu)
{
	V2MP_Word fault;

	if ( !cpu || !cpu->supervisorInterface.fetchInstructionWord )
	{
		return false;
	}

	fault = cpu->supervisorInterface.fetchInstructionWord(cpu->supervisorInterface.supervisor, cpu->pc, &cpu->ir);

	if ( V2MP_CPU_FAULT_CODE(fault) != V2MP_FAULT_NONE )
	{
		V2MP_CPU_NotifyFault(cpu, fault);
		return true;
	}

	cpu->pc += 2;

	return V2MP_CPU_ExecuteInstructionInternal(cpu);
}

void V2MP_CPU_NotifyFault(V2MP_CPU* cpu, V2MP_Fault fault)
{
	if ( !cpu )
	{
		return;
	}

	cpu->fault = fault;
}

bool V2MP_CPU_HasFault(const V2MP_CPU* cpu)
{
	return cpu && V2MP_CPU_FAULT_CODE(cpu->fault) != V2MP_FAULT_NONE;
}

V2MP_Word V2MP_CPU_GetFaultWord(const V2MP_CPU* cpu)
{
	return cpu ? cpu->fault : 0;
}

bool V2MP_CPU_SetRegisterValue(V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value)
{
	V2MP_Word* regPtr;

	if ( !cpu )
	{
		return false;
	}

	regPtr = V2MP_CPU_GetRegisterPtr(cpu, regIndex);

	if ( !regPtr )
	{
		return false;
	}

	*regPtr = value;

	return true;
}

bool V2MP_CPU_SetRegisterValueAndUpdateSR(V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value)
{
	V2MP_Word* regPtr;

	if ( !cpu )
	{
		return false;
	}

	regPtr = V2MP_CPU_GetRegisterPtr(cpu, regIndex);

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

bool V2MP_CPU_GetRegisterValue(const V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word* outValue)
{
	const V2MP_Word* regPtr;

	if ( !cpu || !outValue )
	{
		return false;
	}

	regPtr = V2MP_CPU_GetRegisterConstPtr(cpu, regIndex);

	if ( !regPtr )
	{
		return false;
	}

	*outValue = *regPtr;
	return true;
}

bool V2MP_CPU_ExecuteSingleInstruction(V2MP_CPU* cpu, V2MP_Word instruction)
{
	if ( !cpu )
	{
		return false;
	}

	cpu->ir = instruction;

	return V2MP_CPU_ExecuteInstructionInternal(cpu);
}

V2MP_Word V2MP_CPU_GetProgramCounter(const V2MP_CPU* cpu)
{
	return cpu ? cpu->pc : 0;
}

void V2MP_CPU_SetProgramCounter(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->pc = value;
}

V2MP_Word V2MP_CPU_GetStatusRegister(const V2MP_CPU* cpu)
{
	return cpu ? cpu->sr : 0;
}

void V2MP_CPU_SetStatusRegister(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->sr = value;
}

V2MP_Word V2MP_CPU_GetLinkRegister(const V2MP_CPU* cpu)
{
	return cpu ? cpu->lr : 0;
}

void V2MP_CPU_SetLinkRegister(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->lr = value;
}

V2MP_Word V2MP_CPU_GetR0(const V2MP_CPU* cpu)
{
	return cpu ? cpu->r0 : 0;
}

void V2MP_CPU_SetR0(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->r0 = value;
}

V2MP_Word V2MP_CPU_GetR1(const V2MP_CPU* cpu)
{
	return cpu ? cpu->r1 : 0;
}

void V2MP_CPU_SetR1(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->r1 = value;
}

V2MP_Word V2MP_CPU_GetInstructionRegister(const V2MP_CPU* cpu)
{
	return cpu ? cpu->ir : 0;
}

void V2MP_CPU_SetInstructionRegister(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->ir = value;
}

V2MP_Word V2MP_CPU_GetStackPointer(const V2MP_CPU* cpu)
{
	return cpu ? cpu->sp : 0;
}

void V2MP_CPU_SetStackPointer(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( !cpu )
	{
		return;
	}

	cpu->sp = value;
}
