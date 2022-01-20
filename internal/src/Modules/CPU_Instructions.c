#include <stdlib.h>
#include "Modules/CPU_Instructions.h"
#include "Modules/CPU_Internal.h"
#include "Modules/CPU_DevicePortOperations.h"

// Return value is false under exceptional circumstances
// (eg. CPU was not set up with supervisor interface).
// These circumstances imply a programmer error. Other
// error states that the CPU may legitimately enter into
// (eg. raising a fault) should still result in true
// being returned.
typedef bool (* InstructionCallback)(V2MP_CPU*);

static bool Execute_ADD(V2MP_CPU* cpu);
static bool Execute_SUB(V2MP_CPU* cpu);
static bool Execute_ASGN(V2MP_CPU* cpu);
static bool Execute_SHFT(V2MP_CPU* cpu);
static bool Execute_BITW(V2MP_CPU* cpu);
static bool Execute_CBX(V2MP_CPU* cpu);
static bool Execute_LDST(V2MP_CPU* cpu);
static bool Execute_DPQ(V2MP_CPU* cpu);
static bool Execute_DPO(V2MP_CPU* cpu);
static bool Execute_STK(V2MP_CPU* cpu);
static bool Execute_Unassigned(V2MP_CPU* cpu);
static bool Execute_HCF(V2MP_CPU* cpu);

static const InstructionCallback INSTRUCTION_TABLE[0x10] =
{
	&Execute_ADD,			// 0x0
	&Execute_SUB,			// 0x1
	&Execute_Unassigned,	// 0x2
	&Execute_Unassigned,	// 0x3
	&Execute_ASGN,			// 0x4
	&Execute_SHFT,			// 0x5
	&Execute_BITW,			// 0x6
	&Execute_CBX,			// 0x7
	&Execute_LDST,			// 0x8
	&Execute_DPQ,			// 0x9
	&Execute_DPO,			// 0xA
	&Execute_STK,			// 0xB
	&Execute_Unassigned,	// 0xC
	&Execute_Unassigned,	// 0xD
	&Execute_Unassigned,	// 0xE
	&Execute_HCF			// 0xF
};

static inline void SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args)
{
	V2MP_CPU_NotifyFault(cpu, V2MP_CPU_MAKE_FAULT_WORD(fault, args));
}

static bool Execute_ADDOrSUB(V2MP_CPU* cpu, bool isAdd)
{
	int32_t multiplier;
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;
	V2MP_Word oldValue;

	multiplier = isAdd ? 1 : -1;
	sourceReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_ADDSUB_SREGINDEX(cpu->ir));
	destReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_ADDSUB_DREGINDEX(cpu->ir));
	oldValue = *destReg;

	if ( V2MP_OP_ADDSUB_DREGINDEX(cpu->ir) == V2MP_REGID_PC )
	{
		// Increment or decrement by words, not bytes.
		multiplier *= sizeof(V2MP_Word);
	}

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_ADDSUB_VALUE(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}

		*destReg += (V2MP_Word)(multiplier * (int32_t)(*sourceReg));
	}
	else
	{
		*destReg += (V2MP_Word)(multiplier * (int32_t)((uint8_t)V2MP_OP_ADDSUB_VALUE(cpu->ir)));
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

	return true;
}

bool Execute_ADD(V2MP_CPU* cpu)
{
	return Execute_ADDOrSUB(cpu, true);
}

bool Execute_SUB(V2MP_CPU* cpu)
{
	return Execute_ADDOrSUB(cpu, false);
}

bool Execute_ASGN(V2MP_CPU* cpu)
{
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;

	sourceReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_ASGN_SREGINDEX(cpu->ir));
	destReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_ASGN_DREGINDEX(cpu->ir));

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_ASGN_VALUE(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}

		*destReg = *sourceReg;
	}
	else
	{
		if ( V2MP_OP_ASGN_DREGINDEX(cpu->ir) == V2MP_REGID_PC )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}

		*destReg = (int8_t)V2MP_OP_ASGN_VALUE(cpu->ir);
	}

	cpu->sr = 0;

	if ( *destReg == 0 )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}

	return true;
}

bool Execute_SHFT(V2MP_CPU* cpu)
{
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;
	V2MP_Word rawShiftValue = 0;
	int16_t signedShiftValue = 0;

	if ( V2MP_OP_SHFT_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	sourceReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_SHFT_SREGINDEX(cpu->ir));
	destReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_SHFT_DREGINDEX(cpu->ir));

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_SHFT_VALUE(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}

		rawShiftValue = *sourceReg;
	}
	else
	{
		rawShiftValue = V2MP_OP_SHFT_VALUE(cpu->ir);

		if ( rawShiftValue & 0x0010 )
		{
			// This is a 5-bit value. Bit 4 signifies that it is negative.
			// Construct a 16-bit two's complement for the negative value.
			rawShiftValue = 0xFFF0 | (rawShiftValue & 0x000F);
		}
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

	return true;
}

bool Execute_BITW(V2MP_CPU* cpu)
{
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;
	V2MP_Word bitmask;

	if ( V2MP_OP_BITW_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	sourceReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_BITW_SREGINDEX(cpu->ir));
	destReg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_BITW_DREGINDEX(cpu->ir));

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_BITW_MASKSHIFT(cpu->ir) != 0 ||
		     V2MP_OP_BITW_FLIPMASK(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}

		bitmask = *sourceReg;
	}
	else
	{
		if ( V2MP_OP_BITW_OPTYPE(cpu->ir) == V2MP_BITOP_NOT &&
		     (V2MP_OP_BITW_FLIPMASK(cpu->ir) | V2MP_OP_BITW_MASKSHIFT(cpu->ir)) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}

		bitmask = (1 << V2MP_OP_BITW_MASKSHIFT(cpu->ir));

		if ( V2MP_OP_BITW_FLIPMASK(cpu->ir) )
		{
			bitmask = ~bitmask;
		}
	}

	switch ( V2MP_OP_BITW_OPTYPE(cpu->ir) )
	{
		case V2MP_BITOP_AND:
		{
			*destReg &= bitmask;
			break;
		}

		case V2MP_BITOP_OR:
		{
			*destReg |= bitmask;
			break;
		}

		case V2MP_BITOP_XOR:
		{
			*destReg ^= bitmask;
			break;
		}

		default:
		{
			*destReg = ~(*destReg);
			break;
		}
	}

	cpu->sr = 0;

	if ( *destReg == 0 )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}

	return true;
}

bool Execute_CBX(V2MP_CPU* cpu)
{
	bool shouldBranch;

	if ( V2MP_OP_CBX_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	shouldBranch =
		(V2MP_OP_CBX_BRANCH_ON_SR_Z(cpu->ir) && (cpu->sr & V2MP_CPU_SR_Z)) ||
		(V2MP_OP_CBX_BRANCH_ON_SR_C(cpu->ir) && (cpu->sr & V2MP_CPU_SR_C));

	if ( V2MP_OP_CBX_LR_IS_TARGET(cpu->ir) )
	{
		if ( V2MP_OP_CBX_OFFSET(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}

		if ( shouldBranch )
		{
			cpu->pc = cpu->lr;
		}
	}
	else
	{
		if ( shouldBranch )
		{
			cpu->pc += sizeof(V2MP_Word) * (int8_t)V2MP_OP_CBX_OFFSET(cpu->ir);
		}
	}

	cpu->sr = 0;

	if ( !shouldBranch )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}

	return true;
}

bool Execute_LDST(V2MP_CPU* cpu)
{
	V2MP_Word* reg;

	if ( !cpu->supervisorInterface.requestLoadWordFromDS ||
	     !cpu->supervisorInterface.requestStoreWordToDS )
	{
		return false;
	}

	if ( V2MP_OP_LDST_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	reg = V2MP_CPU_GetRegisterPtr(cpu, V2MP_OP_LDST_REGINDEX(cpu->ir));

	if ( V2MP_OP_LDST_IS_STORE(cpu->ir) )
	{
		cpu->supervisorInterface.requestStoreWordToDS(
			cpu->supervisorInterface.supervisor,
			cpu->lr,
			*reg
		);

		cpu->sr = 0;

		if ( *reg == 0 )
		{
			cpu->sr |= V2MP_CPU_SR_Z;
		}
	}
	else
	{
		cpu->supervisorInterface.requestLoadWordFromDS(
			cpu->supervisorInterface.supervisor,
			cpu->lr,
			(V2MP_RegisterIndex)V2MP_OP_LDST_REGINDEX(cpu->ir)
		);

		// The status register will be updated later.
	}

	return true;
}

bool Execute_DPQ(V2MP_CPU* cpu)
{
	if ( !cpu->supervisorInterface.performDevicePortQuery )
	{
		return false;
	}

	if ( V2MP_OP_DPQ_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	switch ( V2MP_OP_DPQ_QUERY_TYPE(cpu->ir) )
	{
		case V2MP_DPQT_CONNECTED:
		case V2MP_DPQT_READABLE_NOT_BUSY:
		case V2MP_DPQT_WRITEABLE_NOT_BUSY:
		case V2MP_DPQT_EXHAUSTED:
		case V2MP_DPQT_BUSY:
		case V2MP_DPQT_CONTROLLED_BY_PROGRAM:
		{
			cpu->supervisorInterface.performDevicePortQuery(
				cpu->supervisorInterface.supervisor,
				cpu->r0,
				V2MP_OP_DPQ_QUERY_TYPE(cpu->ir)
			);

			break;
		}

		default:
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			break;
		}
	}

	return true;
}

bool Execute_DPO(V2MP_CPU* cpu)
{
	if ( V2MP_OP_DPO_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	switch ( V2MP_OP_DPO_OPERATION_TYPE(cpu->ir) )
	{
		case V2MP_DPOT_USABLE_BYTE_COUNT:
		{
			return V2MP_CPU_DPO_UsableByteCount(cpu);
		}

		case V2MP_DPOT_RELINQUISH_MAILBOX:
		{
			return V2MP_CPU_DPO_RelinquishMailbox(cpu);
		}

		case V2MP_DPOT_READ:
		{
			return V2MP_OP_DPO_INDIRECT_TRANSFER(cpu->ir)
				? V2MP_CPU_DPO_Read_IDT(cpu)
				: V2MP_CPU_DPO_Read_DDT(cpu);
		}

		case V2MP_DPOT_WRITE:
		{
			return V2MP_OP_DPO_INDIRECT_TRANSFER(cpu->ir)
				? V2MP_CPU_DPO_Write_IDT(cpu)
				: V2MP_CPU_DPO_Write_DDT(cpu);
		}

		default:
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return true;
		}
	}
}

static bool Execute_STK(V2MP_CPU* cpu)
{
	V2MP_Word regFlags = 0;

	if ( !cpu->supervisorInterface.requestStackPush ||
	     !cpu->supervisorInterface.requestStackPop )
	{
		return false;
	}

	if ( V2MP_OP_STK_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	if ( V2MP_OP_STK_R0(cpu->ir) )
	{
		regFlags |= (1 << V2MP_REGID_R0);
	}

	if ( V2MP_OP_STK_R1(cpu->ir) )
	{
		regFlags |= (1 << V2MP_REGID_R1);
	}

	if ( V2MP_OP_STK_LR(cpu->ir) )
	{
		regFlags |= (1 << V2MP_REGID_LR);
	}

	if ( V2MP_OP_STK_PC(cpu->ir) )
	{
		regFlags |= (1 << V2MP_REGID_PC);
	}

	if ( regFlags == 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return true;
	}

	if ( V2MP_OP_STK_PUSH(cpu->ir) )
	{
		cpu->supervisorInterface.requestStackPush(
			cpu->supervisorInterface.supervisor,
			regFlags
		);
	}
	else
	{
		cpu->supervisorInterface.requestStackPop(
			cpu->supervisorInterface.supervisor,
			regFlags
		);
	}

	return true;
}

bool Execute_Unassigned(V2MP_CPU* cpu)
{
	SetFault(cpu, V2MP_FAULT_INI, cpu->ir >> 12);
	return true;
}

bool Execute_HCF(V2MP_CPU* cpu)
{
	SetFault(cpu, V2MP_FAULT_HCF, cpu->ir);
	return true;
}

bool V2MP_CPU_ExecuteInstructionInternal(V2MP_CPU* cpu)
{
	InstructionCallback instructionToExecute = NULL;

	if ( !cpu )
	{
		return false;
	}

	instructionToExecute = INSTRUCTION_TABLE[V2MP_OPCODE(cpu->ir)];

	if ( !instructionToExecute )
	{
		return false;
	}

	return (*instructionToExecute)(cpu);
}
