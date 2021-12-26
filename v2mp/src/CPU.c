#include <string.h>
#include <stdlib.h>
#include "V2MP/CPU.h"
#include "V2MP/MemoryStore.h"
#include "V2MP/DevicePortStore.h"
#include "V2MP/DevicePort.h"
#include "V2MPInternal/Util/Util.h"
#include "V2MPInternal/Util/Heap.h"

struct V2MP_CPU
{
	V2MP_Word pc;
	V2MP_Word sr;
	V2MP_Word lr;
	V2MP_Word r0;
	V2MP_Word r1;
	V2MP_Word ir;
	V2MP_Word fault;

	V2MP_MemoryStore* memory;
	V2MP_DevicePortStore* devicePorts;
};

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

static void SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args)
{
	cpu->fault = V2MP_CPU_MAKE_FAULT_WORD(fault, args);
}

static V2MP_Word* GetRegisterPtr(V2MP_CPU* cpu, V2MP_Word regIndex)
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

static inline bool CPUHasAllPeripherals(V2MP_CPU* cpu)
{
	return cpu && cpu->memory && cpu->devicePorts;
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
			return;
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
}

void Execute_BITW(V2MP_CPU* cpu)
{
	V2MP_Word* sourceReg;
	V2MP_Word* destReg;
	V2MP_Word bitmask;

	if ( V2MP_OP_BITW_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return;
	}

	sourceReg = GetRegisterPtr(cpu, V2MP_OP_BITW_SREGINDEX(cpu->ir));
	destReg = GetRegisterPtr(cpu, V2MP_OP_BITW_DREGINDEX(cpu->ir));

	if ( sourceReg != destReg )
	{
		if ( V2MP_OP_BITW_MASKSHIFT(cpu->ir) != 0 ||
		     V2MP_OP_BITW_FLIPMASK(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return;
		}

		bitmask = *sourceReg;
	}
	else
	{
		if ( V2MP_OP_BITW_OPTYPE(cpu->ir) == V2MP_BITOP_NOT &&
		     (V2MP_OP_BITW_FLIPMASK(cpu->ir) | V2MP_OP_BITW_MASKSHIFT(cpu->ir)) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return;
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
}

void Execute_CBX(V2MP_CPU* cpu)
{
	bool shouldBranch;

	if ( V2MP_OP_CBX_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return;
	}

	shouldBranch =
		(V2MP_OP_CBX_BRANCH_ON_SR_Z(cpu->ir) && (cpu->sr & V2MP_CPU_SR_Z)) ||
		(V2MP_OP_CBX_BRANCH_ON_SR_C(cpu->ir) && (cpu->sr & V2MP_CPU_SR_C));

	if ( V2MP_OP_CBX_LR_IS_TARGET(cpu->ir) )
	{
		if ( V2MP_OP_CBX_OFFSET(cpu->ir) != 0 )
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return;
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
}

void Execute_DPQ(V2MP_CPU* cpu)
{
	V2MP_DevicePort* port = NULL;
	bool result = false;

	if ( V2MP_OP_DPQ_RESBITS(cpu->ir) != 0 )
	{
		SetFault(cpu, V2MP_FAULT_RES, 0);
		return;
	}

	port = V2MP_DevicePortStore_GetPort(cpu->devicePorts, cpu->r0);

	switch ( V2MP_OP_DPQ_QUERY_TYPE(cpu->ir) )
	{
		case V2MP_DPQT_CONNECTED:
		{
			result = port != NULL;
			break;
		}

		case V2MP_DPQT_EXHAUSTED:
		{
			result = port ? V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED : false;
			break;
		}

		// TODO: Others

		default:
		{
			SetFault(cpu, V2MP_FAULT_RES, 0);
			return;
		}
	}

	cpu->sr = 0;

	if ( !result )
	{
		cpu->sr |= V2MP_CPU_SR_Z;
	}
}

void Execute_DPO(V2MP_CPU* cpu)
{
	// TODO:
	SetFault(cpu, V2MP_FAULT_INI, cpu->ir >> 12);
}

void Execute_Unassigned(V2MP_CPU* cpu)
{
	SetFault(cpu, V2MP_FAULT_INI, cpu->ir >> 12);
}

size_t V2MP_CPU_Footprint(void)
{
	return sizeof(V2MP_CPU);
}

V2MP_CPU* V2MP_CPU_AllocateAndInit(void)
{
	return V2MPI_CALLOC_STRUCT(V2MP_CPU);
}

void V2MP_CPU_DeinitAndFree(V2MP_CPU* cpu)
{
	if ( !cpu )
	{
		return;
	}

	// Nothing to deinit yet.

	free(cpu);
}

void V2MP_CPU_Reset(V2MP_CPU* cpu)
{
	if ( !cpu )
	{
		return;
	}

	// From a defensive programming point of view, even though
	// it would be quicker to stash the memory store pointer,
	// wipe the struct and reapply the pointer, zeroing only
	// the members we want is a safer approach.

	cpu->pc = 0;
	cpu->sr = 0;
	cpu->lr = 0;
	cpu->r0 = 0;
	cpu->r1 = 0;
	cpu->ir = 0;
	cpu->fault = 0;
}

bool V2MP_CPU_FetchDecodeAndExecuteInstruction(V2MP_CPU* cpu)
{
	V2MP_Fault fault = 0;
	InstructionCallback instructionToExecute = NULL;

	if ( !CPUHasAllPeripherals(cpu) )
	{
		return false;
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

bool V2MP_CPU_ExecuteInstruction(V2MP_CPU* cpu, V2MP_Word instruction)
{
	InstructionCallback instructionToExecute = NULL;

	if ( !CPUHasAllPeripherals(cpu) )
	{
		return false;
	}

	cpu->ir = instruction;
	instructionToExecute = INSTRUCTION_TABLE[V2MP_OPCODE(cpu->ir)];

	if ( !instructionToExecute )
	{
		return false;
	}

	(*instructionToExecute)(cpu);
	return true;
}

struct V2MP_MemoryStore* V2MP_CPU_GetMemoryStore(V2MP_CPU* cpu)
{
	return cpu ? cpu->memory : NULL;
}

void V2MP_CPU_SetMemoryStore(V2MP_CPU* cpu, struct V2MP_MemoryStore* memory)
{
	if ( cpu )
	{
		cpu->memory = memory;
	}
}

struct V2MP_DevicePortStore* V2MP_CPU_GetDevicePortStore(V2MP_CPU* cpu)
{
	return cpu ? cpu->devicePorts : NULL;
}

void V2MP_CPU_SetDevicePortStore(V2MP_CPU* cpu, struct V2MP_DevicePortStore* devicePorts)
{
	if ( cpu )
	{
		cpu->devicePorts = devicePorts;
	}
}

V2MP_Word V2MP_CPU_GetProgramCounter(const V2MP_CPU* cpu)
{
	return cpu ? cpu->pc : 0;
}

void V2MP_CPU_SetProgramCounter(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( cpu )
	{
		cpu->pc = value;
	}
}

V2MP_Word V2MP_CPU_GetStatusRegister(const V2MP_CPU* cpu)
{
	return cpu ? cpu->sr : 0;
}

void V2MP_CPU_SetStatusRegister(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( cpu )
	{
		cpu->sr = value;
	}
}

V2MP_Word V2MP_CPU_GetLinkRegister(const V2MP_CPU* cpu)
{
	return cpu ? cpu->lr : 0;
}

void V2MP_CPU_SetLinkRegister(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( cpu )
	{
		cpu->lr = value;
	}
}

V2MP_Word V2MP_CPU_GetR0(const V2MP_CPU* cpu)
{
	return cpu ? cpu->r0 : 0;
}

void V2MP_CPU_SetR0(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( cpu )
	{
		cpu->r0 = value;
	}
}

V2MP_Word V2MP_CPU_GetR1(const V2MP_CPU* cpu)
{
	return cpu ? cpu->r1 : 0;
}

void V2MP_CPU_SetR1(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( cpu )
	{
		cpu->r1 = value;
	}
}

V2MP_Word V2MP_CPU_GetInstructionRegister(const V2MP_CPU* cpu)
{
	return cpu ? cpu->ir : 0;
}

void V2MP_CPU_SetInstructionRegister(V2MP_CPU* cpu, V2MP_Word value)
{
	if ( cpu )
	{
		cpu->ir = value;
	}
}

V2MP_Word V2MP_CPU_GetFault(const V2MP_CPU* cpu)
{
	return cpu ? cpu->fault : 0;
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
