#include <string.h>
#include "V2MP/CPU.h"
#include "V2MP/MemoryStore.h"

typedef void (* InstructionCallback)(V2MP_CPU*);

static void Execute_HCF(V2MP_CPU* cpu);
static void Execute_LDST(V2MP_CPU* cpu);
static void Execute_ASGN(V2MP_CPU* cpu);
static void Execute_ADD(V2MP_CPU* cpu);
static void Execute_SUB(V2MP_CPU* cpu);
static void Execute_SHFT(V2MP_CPU* cpu);
static void Execute_BITW(V2MP_CPU* cpu);
static void Execute_CBX(V2MP_CPU* cpu);
static void Execute_DPQ(V2MP_CPU* cpu);
static void Execute_DPO(V2MP_CPU* cpu);
static void Execute_Unassigned(V2MP_CPU* cpu);
static void SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args);

static const InstructionCallback INSTRUCTION_TABLE[0x10] =
{
	&Execute_HCF,			// 0x0
	&Execute_LDST,			// 0x1
	&Execute_ASGN,			// 0x2
	&Execute_ADD,			// 0x3
	&Execute_SUB,			// 0x4
	&Execute_SHFT,			// 0x5
	&Execute_BITW,			// 0x6
	&Execute_CBX,			// 0x7
	&Execute_DPQ,			// 0x8
	&Execute_DPO,			// 0x9
	&Execute_Unassigned,	// 0xA
	&Execute_Unassigned,	// 0xB
	&Execute_Unassigned,	// 0xC
	&Execute_Unassigned,	// 0xD
	&Execute_Unassigned,	// 0xE
	&Execute_Unassigned		// 0xF
};

void Execute_HCF(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_LDST(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_ASGN(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_ADD(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_SUB(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_SHFT(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_BITW(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_CBX(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_DPQ(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_DPO(V2MP_CPU* cpu)
{
	// TODO:
	cpu->fault = V2MP_FAULT_INV << 12;
}

void Execute_Unassigned(V2MP_CPU* cpu)
{
	cpu->fault = (V2MP_FAULT_INV << 12) | (cpu->ir >> 12);
}

void SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args)
{
	cpu->fault = (fault << 12) | (args & 0x0FFF);
}

void V2MP_CPU_Init(V2MP_CPU* cpu)
{
	if ( !cpu )
	{
		return;
	}

	memset(cpu, 0, sizeof(*cpu));
}

void V2MP_CPU_Deinit(V2MP_CPU* cpu)
{
	if ( !cpu )
	{
		return;
	}

	// Nothing to do yet.
}

bool V2MP_CPU_FetchDecodeAndExecute(V2MP_CPU* cpu)
{
	V2MP_Fault fault = 0;
	InstructionCallback instructionToExecute = NULL;

	if ( !cpu || !cpu->memory )
	{
		return false;
	}

	if ( cpu->pc & 1 )
	{
		SetFault(cpu, V2MP_FAULT_ALGN, 0);
		return true;
	}

	if ( !V2MP_MemoryStore_FetchCSWord(cpu->memory, cpu->pc, &cpu->ir, &fault) )
	{
		return false;
	}

	if ( fault != V2MP_FAULT_NONE )
	{
		SetFault(cpu, fault, 0);
		return true;
	}

	cpu->pc += 2;

	instructionToExecute = INSTRUCTION_TABLE[cpu->ir >> 12];

	if ( !instructionToExecute )
	{
		return false;
	}

	(*instructionToExecute)(cpu);
	return true;
}
