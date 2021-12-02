#include <string.h>
#include "V2MP/CPU.h"
#include "V2MP/MemoryStore.h"

typedef void (* InstructionCallback)(V2MP_CPU*);

static void SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args);
static V2MP_Word* GetRegisterPtr(V2MP_CPU* cpu, V2MP_Word regIndex);

static void Execute_HCF(V2MP_CPU* cpu);
static void Execute_LDST(V2MP_CPU* cpu);
static void Execute_ASGN(V2MP_CPU* cpu);
static void Execute_ADDOrSUB(V2MP_CPU* cpu);
static void Execute_SHFT(V2MP_CPU* cpu);
static void Execute_BITW(V2MP_CPU* cpu);
static void Execute_CBX(V2MP_CPU* cpu);
static void Execute_DPQ(V2MP_CPU* cpu);
static void Execute_DPO(V2MP_CPU* cpu);
static void Execute_Unassigned(V2MP_CPU* cpu);

static const InstructionCallback INSTRUCTION_TABLE[0x10] =
{
	&Execute_HCF,			// 0x0
	&Execute_LDST,			// 0x1
	&Execute_ASGN,			// 0x2
	&Execute_ADDOrSUB,		// 0x3
	&Execute_ADDOrSUB,		// 0x4
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

void SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args)
{
	cpu->fault = (fault << 12) | (args & 0x0FFF);
}

V2MP_Word* GetRegisterPtr(V2MP_CPU* cpu, V2MP_Word regIndex)
{
	switch ( V2MP_REGID_MASK(regIndex) )
	{
		case V2MP_REGID_R0:
		{
			return &cpu->r0;
		}

		case V2MP_REGID_R1:
		{
			return &cpu->r1;
		}

		case V2MP_REGID_LR:
		{
			return &cpu->lr;
		}

		default:
		{
			return &cpu->pc;
		}
	}
}

void Execute_HCF(V2MP_CPU* cpu)
{
	SetFault(cpu, V2MP_FAULT_HCF, cpu->ir);
}

void Execute_LDST(V2MP_CPU* cpu)
{
	V2MP_Word* reg;
	V2MP_Fault fault = V2MP_FAULT_NONE;

	if ( V2MP_OP_LDST_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return;
	}

	reg = GetRegisterPtr(cpu, V2MP_OP_LDST_REGINDEX(cpu->ir));

	if ( V2MP_OP_LDST_IS_STORE(cpu->ir) )
	{
		V2MP_MemoryStore_StoreDSWord(cpu->memory, cpu->lr, *reg, &fault);
	}
	else
	{
		V2MP_MemoryStore_FetchDSWord(cpu->memory, cpu->lr, reg, &fault);
	}

	SetFault(cpu, fault, 0);

	if ( cpu->fault )
	{
		return;
	}

	cpu->sr = 0;

	if ( *reg == 0 )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}
}

void Execute_ASGN(V2MP_CPU* cpu)
{
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;

	sourceReg = GetRegisterPtr(cpu, V2MP_OP_ASGN_SREGINDEX(cpu->ir));
	destReg = GetRegisterPtr(cpu, V2MP_OP_ASGN_DREGINDEX(cpu->ir));

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_ASGN_VALUE(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return;
		}

		*destReg = *sourceReg;
	}
	else
	{
		if ( V2MP_OP_ASGN_DREGINDEX(cpu->ir) == V2MP_REGID_PC )
		{
			SetFault(cpu, V2MP_FAULT_INO, 0);
			return;
		}

		*destReg = (int8_t)V2MP_OP_ASGN_VALUE(cpu->ir);
	}

	cpu->sr = 0;

	if ( *destReg == 0 )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}
}

void Execute_ADDOrSUB(V2MP_CPU* cpu)
{
	int32_t multiplier;
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;
	V2MP_Word oldValue;

	multiplier = V2MP_OPCODE(cpu->ir) == V2MP_OP_ADD ? 1 : -1;
	sourceReg = GetRegisterPtr(cpu, V2MP_OP_ADDSUB_SREGINDEX(cpu->ir));
	destReg = GetRegisterPtr(cpu, V2MP_OP_ADDSUB_DREGINDEX(cpu->ir));
	oldValue = *destReg;

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_ADDSUB_VALUE(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return;
		}

		*destReg += multiplier * (int32_t)(*sourceReg);
	}
	else
	{
		if ( V2MP_OP_ADDSUB_DREGINDEX(cpu->ir) == V2MP_REGID_PC )
		{
			*destReg += 2 * multiplier * (int32_t)((uint8_t)V2MP_OP_ADDSUB_VALUE(cpu->ir));
		}
		else
		{
			*destReg += multiplier * (int32_t)((uint8_t)V2MP_OP_ADDSUB_VALUE(cpu->ir));
		}
	}

	cpu->sr = 0;

	if ( (V2MP_OPCODE(cpu->ir) == V2MP_OP_ADD && *destReg < oldValue) ||
	     (V2MP_OPCODE(cpu->ir) == V2MP_OP_SUB && *destReg > oldValue) )
	{
		cpu->sr |= V2MP_CPU_SR_C;
	}

	if ( *destReg == 0 )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}
}

void Execute_SHFT(V2MP_CPU* cpu)
{
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;
	V2MP_Word rawShiftValue = 0;
	int16_t signedShiftValue = 0;

	if ( V2MP_OP_SHFT_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return;
	}

	sourceReg = GetRegisterPtr(cpu, V2MP_OP_SHFT_SREGINDEX(cpu->ir));
	destReg = GetRegisterPtr(cpu, V2MP_OP_SHFT_DREGINDEX(cpu->ir));

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_SHFT_VALUE(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return;
		}

		rawShiftValue = V2MP_OP_SHFT_MAGMASK(*sourceReg);
	}
	else
	{
		rawShiftValue = V2MP_OP_SHFT_VALUE(cpu->ir);
	}

	if ( rawShiftValue & 0x0010 )
	{
		// This is a 5-bit value. Bit 4 signifies that it is negative.
		// Construct a 16-bit two's complement for the negative value.
		rawShiftValue = 0xFFE0 | (rawShiftValue & 0x000F);
	}

	// Actually interpret these bits as signed now.
	signedShiftValue = *((int16_t*)&rawShiftValue);

	cpu->sr = 0;

	// To check whether any bits got shifted off the end,
	// we can mask the original value with the span of bits
	// that did get shifted off the end. If the intersection
	// of these two values is not zero, bits in the register
	// were shifted off the end.

	if ( signedShiftValue < 0 )
	{
		signedShiftValue *= -1;

		if ( *destReg & (0xFFFF >> (16 - signedShiftValue)) )
		{
			cpu->sr |= V2MP_CPU_SR_C;
		}

		*destReg >>= signedShiftValue;
	}
	else
	{
		if ( *destReg & (0xFFFF << (16 - signedShiftValue)) )
		{
			cpu->sr |= V2MP_CPU_SR_C;
		}

		*destReg <<= signedShiftValue;
	}

	if ( *destReg == 0 )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}
}

void Execute_BITW(V2MP_CPU* cpu)
{
	// TODO:
	SetFault(cpu, V2MP_FAULT_INO, cpu->ir >> 12);
}

void Execute_CBX(V2MP_CPU* cpu)
{
	// TODO:
	SetFault(cpu, V2MP_FAULT_INO, cpu->ir >> 12);
}

void Execute_DPQ(V2MP_CPU* cpu)
{
	// TODO:
	SetFault(cpu, V2MP_FAULT_INO, cpu->ir >> 12);
}

void Execute_DPO(V2MP_CPU* cpu)
{
	// TODO:
	SetFault(cpu, V2MP_FAULT_INO, cpu->ir >> 12);
}

void Execute_Unassigned(V2MP_CPU* cpu)
{
	SetFault(cpu, V2MP_FAULT_INO, cpu->ir >> 12);
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

	instructionToExecute = INSTRUCTION_TABLE[V2MP_OPCODE(cpu->ir)];

	if ( !instructionToExecute )
	{
		return false;
	}

	(*instructionToExecute)(cpu);
	return true;
}

bool V2MP_CPU_SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args)
{
	if ( !cpu )
	{
		return false;
	}

	SetFault(cpu, fault, args);
	return true;
}
