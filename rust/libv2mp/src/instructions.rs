use super::arch::*;
use super::cpu::Registers;

pub fn execute(instruction: InstructionWord, registers: Registers) -> Registers
{
	let index: usize = OpCode::from_instruction(instruction).as_value() as usize;
	debug_assert!(index < NUM_OPCODES);

	return INSTRUCTION_CALLBACKS[index](instruction, &registers).apply(registers);
}

enum AddOrSub
{
	Add,
	Sub,
}

struct InstructionResult
{
	pub pc: Option<Word>,
	pub sr: Option<Word>,
	pub lr: Option<Word>,
	pub r0: Option<Word>,
	pub r1: Option<Word>,
	pub ir: Option<Word>,
	pub sp: Option<Word>,
	pub fault: Option<Word>,
}

impl InstructionResult
{
	pub fn set_register(mut self, index: RegisterIndex, value: Word) -> Self
	{
		match index
		{
			RegisterIndex::R0 => self.r0 = Some(value),
			RegisterIndex::R1 => self.r1 = Some(value),
			RegisterIndex::Lr => self.lr = Some(value),
			RegisterIndex::Pc => self.pc = Some(value),
		};

		return self;
	}
}

type InstructionCallback =
	fn(instruction: InstructionWord, registers: &Registers) -> InstructionResult;

static INSTRUCTION_CALLBACKS: [InstructionCallback; NUM_OPCODES] = [
	executeNop,        // 0x00 Nop
	executeAdd,        // 0x01 Add
	executeSub,        // 0x02 Sub
	executeUnassigned, // 0x03 Mul
	executeUnassigned, // 0x04 Div
	executeUnassigned, // 0x05 Asgn
	executeUnassigned, // 0x06 Shft
	executeUnassigned, // 0x07 Bitw
	executeUnassigned, // 0x08 Cbx
	executeUnassigned, // 0x09 Ldst
	executeUnassigned, // 0x0A Stk
	executeUnassigned, // 0x0B Sig
	executeUnassigned, // 0x0C Unassigned0
	executeUnassigned, // 0x0D Unassigned1
	executeUnassigned, // 0x0E Unassigned2
	executeUnassigned, // 0x0F Unassigned3
];

impl InstructionResult
{
	pub fn apply(self, registers: Registers) -> Registers
	{
		return Registers {
			pc: self.pc.unwrap_or(registers.pc),
			sr: self.sr.unwrap_or(registers.sr),
			lr: self.lr.unwrap_or(registers.lr),
			r0: self.r0.unwrap_or(registers.r0),
			r1: self.r1.unwrap_or(registers.r1),
			ir: self.ir.unwrap_or(registers.ir),
			sp: self.sp.unwrap_or(registers.sp),
			fault: self.fault.unwrap_or(registers.fault),
		};
	}
}

impl Default for InstructionResult
{
	fn default() -> Self
	{
		return Self {
			pc: None,
			sr: None,
			lr: None,
			r0: None,
			r1: None,
			ir: None,
			sp: None,
			fault: None,
		};
	}
}

fn executeUnassigned(_: InstructionWord, _: &Registers) -> InstructionResult
{
	return InstructionResult {
		fault: Some(FaultCode::Ini.as_register_value(0)),
		..Default::default()
	};
}

fn executeNop(instruction: InstructionWord, _: &Registers) -> InstructionResult
{
	return InstructionResult {
		// Fault if any of the arg bits are set
		fault: faultRegisterIfReservedBitsSet(instruction, INSTRUCTION_ARG_MASK),
		..Default::default()
	};
}

fn executeAdd(instruction: InstructionWord, registers: &Registers) -> InstructionResult
{
	return executeAddOrSub(instruction, registers, AddOrSub::Add);
}

fn executeSub(instruction: InstructionWord, registers: &Registers) -> InstructionResult
{
	return executeAddOrSub(instruction, registers, AddOrSub::Add);
}

fn executeAddOrSub(
	instruction: InstructionWord,
	registers: &Registers,
	operation: AddOrSub,
) -> InstructionResult
{
	const SRC_REG_IDX_OFFSET: u8 = 10;
	const DEST_REG_IDX_OFFSET: u8 = 8;
	const LITERAL_MASK: Word = 0x00FF;
	const LITERAL_OFFSET: u8 = 0;

	let src_reg: RegisterIndex = RegisterIndex::from_instruction(instruction, SRC_REG_IDX_OFFSET);
	let dest_reg: RegisterIndex = RegisterIndex::from_instruction(instruction, DEST_REG_IDX_OFFSET);
	let literal: Word = instruction.bits(LITERAL_MASK, 0, LITERAL_OFFSET);

	if src_reg != dest_reg && literal != 0
	{
		// Should not have a literal value here, since
		// the source register is being used as a value.
		return InstructionResult {
			fault: Some(FaultCode::Res.as_register_value(0)),
			..Default::default()
		};
	}

	let stride: Word = (if dest_reg == RegisterIndex::Pc
	{
		SIZE_OF_WORD
	}
	else
	{
		SIZE_OF_BYTE
	}) as Word;

	let rhs: Word = if src_reg != dest_reg
	{
		registers.get_register_value(src_reg)
	}
	else
	{
		literal
	};

	let lhs: Word = registers.get_register_value(dest_reg);

	let op_result: Word = match operation
	{
		AddOrSub::Add => lhs.wrapping_add(stride * rhs),
		AddOrSub::Sub => lhs.wrapping_sub(stride * rhs),
	};

	let overflowed: bool = match operation
	{
		AddOrSub::Add => op_result < lhs,
		AddOrSub::Sub => op_result > lhs,
	};

	let status_result: Word = 0;
	let status_result: Word = StatusRegisterFlag::C.set_if(status_result, overflowed);
	let status_result: Word = StatusRegisterFlag::Z.set_if(status_result, op_result == 0);

	return InstructionResult {
		sr: Some(status_result),
		..Default::default()
	}
	.set_register(dest_reg, op_result);
}

fn faultRegisterIfReservedBitsSet(instruction: InstructionWord, reserved_mask: Word)
-> Option<Word>
{
	return if instruction.any_bits_set(reserved_mask)
	{
		Some(FaultCode::Res.as_register_value(0))
	}
	else
	{
		None
	};
}
