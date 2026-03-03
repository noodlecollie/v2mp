use super::arch::*;
use super::cpu::Registers;

pub fn execute(instruction: InstructionWord, registers: Registers) -> Registers
{
	let index: usize = OpCode::from_instruction(instruction).as_value() as usize;
	debug_assert!(index < NUM_OPCODES);

	return INSTRUCTION_CALLBACKS[index](instruction, &registers).apply(registers);
}

const REG_VAL_FAULT_RES: Word = FaultCode::Res.as_register_value(0);
const REG_VAL_FAULT_DIV: Word = FaultCode::Div.as_register_value(0);

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

struct MulDivParams
{
	pub source_value: Word,
	pub dest_reg: RegisterIndex,
	pub operation_is_signed: bool,
}

impl MulDivParams
{
	pub fn get(instruction: InstructionWord, registers: &Registers) -> Self
	{
		const MASK_STATIC: Word = 0x0400;
		const MASK_LITERAL: Word = 0x00FF;
		const MASK_DEST_REG_IS_R1: Word = 0x0800;
		const MASK_SIGNED: Word = 0x0200;

		let is_signed: bool = instruction.any_bits_set(MASK_SIGNED);

		let dest_reg: RegisterIndex = if instruction.any_bits_set(MASK_DEST_REG_IS_R1)
		{
			RegisterIndex::R1
		}
		else
		{
			RegisterIndex::R0
		};

		let source_value: Word = if instruction.any_bits_set(MASK_STATIC)
		{
			// TODO: Does this actually make a difference?
			if is_signed
			{
				(instruction.value() & MASK_LITERAL) as i8 as Word
			}
			else
			{
				(instruction.value() & MASK_LITERAL) as u8 as Word
			}
		}
		else
		{
			registers.get_register_value(
				if dest_reg == RegisterIndex::R0
				{
					RegisterIndex::R1
				}
				else
				{
					RegisterIndex::R0
				},
			)
		};

		return Self {
			source_value,
			dest_reg,
			operation_is_signed: is_signed,
		};
	}
}

type InstructionCallback =
	fn(instruction: InstructionWord, registers: &Registers) -> InstructionResult;

static INSTRUCTION_CALLBACKS: [InstructionCallback; NUM_OPCODES] = [
	executeNop,        // 0x00 Nop
	executeAdd,        // 0x01 Add
	executeSub,        // 0x02 Sub
	executeMul,        // 0x03 Mul
	executeDiv,        // 0x04 Div
	executeAsgn,       // 0x05 Asgn
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

fn executeMul(instruction: InstructionWord, registers: &Registers) -> InstructionResult
{
	const MASK_RESBITS: Word = 0x0100;

	struct MulResult
	{
		upper: Word,
		lower: Word,
		overflowed: bool,
	}

	if instruction.any_bits_set(MASK_RESBITS)
	{
		return InstructionResult {
			fault: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	let params: MulDivParams = MulDivParams::get(instruction, registers);
	let dest_value: Word = registers.get_register_value(params.dest_reg);

	let op_result: MulResult = if params.operation_is_signed
	{
		let signed_result: i32 = (params.source_value as i32) * (dest_value as i32);
		let unsigned_result: u32 = signed_result as u32;
		let upper: Word = ((unsigned_result & 0xFFFF0000) >> 16) as Word;
		let lower: Word = (unsigned_result & 0x0000FFFF) as Word;
		let overflowed: bool = if signed_result > 0
		{
			signed_result > (i16::MAX as i32)
		}
		else
		{
			signed_result < (i16::MIN as i32)
		};

		MulResult {
			upper,
			lower,
			overflowed,
		}
	}
	else
	{
		let unsigned_result: u32 = (params.source_value as u32) * (dest_value as u32);
		let upper: Word = ((unsigned_result & 0xFFFF0000) >> 16) as Word;
		let lower: Word = (unsigned_result & 0x0000FFFF) as Word;
		let overflowed: bool = upper != 0;

		MulResult {
			upper,
			lower,
			overflowed,
		}
	};

	let status_result: Word = 0;
	let status_result: Word = StatusRegisterFlag::C.set_if(status_result, op_result.overflowed);
	let status_result: Word =
		StatusRegisterFlag::Z.set_if(status_result, op_result.upper == 0 && op_result.lower == 0);

	return InstructionResult {
		sr: Some(status_result),
		..Default::default()
	}
	.set_register(RegisterIndex::Lr, op_result.upper)
	.set_register(params.dest_reg, op_result.lower);
}

fn executeDiv(instruction: InstructionWord, registers: &Registers) -> InstructionResult
{
	const MASK_RESBITS: Word = 0x0100;

	if instruction.any_bits_set(MASK_RESBITS)
	{
		return InstructionResult {
			fault: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	let params: MulDivParams = MulDivParams::get(instruction, registers);

	if params.source_value == 0
	{
		return InstructionResult {
			fault: Some(REG_VAL_FAULT_DIV),
			..Default::default()
		};
	}

	let op_result: (Word, Word) = if params.operation_is_signed
	{
		let numerator: i16 = registers.get_register_value(params.dest_reg) as i16;
		let denominator: i16 = params.source_value as i16;
		let upper: i16 = numerator / denominator;
		let lower: i16 = numerator % denominator;

		(upper as Word, lower as Word)
	}
	else
	{
		let numerator: Word = registers.get_register_value(params.dest_reg);
		let denominator: &Word = &params.source_value;
		let upper: Word = numerator / denominator;
		let lower: Word = numerator % denominator;

		(upper, lower)
	};

	let status_result: Word = 0;
	let status_result: Word = StatusRegisterFlag::C.set_if(status_result, op_result.0 != 0);
	let status_result: Word = StatusRegisterFlag::Z.set_if(status_result, op_result.1 == 0);

	return InstructionResult {
		sr: Some(status_result),
		..Default::default()
	}
	.set_register(RegisterIndex::Lr, op_result.0)
	.set_register(params.dest_reg, op_result.1);
}

fn executeAsgn(instruction: InstructionWord, registers: &Registers) -> InstructionResult
{
	const SRC_REG_IDX_OFFSET: u8 = 10;
	const DEST_REG_IDX_OFFSET: u8 = 8;
	const MASK_LITERAL: Word = 0x00FF;
	const LITERAL_OFFSET: u8 = 0;

	let src_reg: RegisterIndex = RegisterIndex::from_instruction(instruction, SRC_REG_IDX_OFFSET);
	let dest_reg: RegisterIndex = RegisterIndex::from_instruction(instruction, DEST_REG_IDX_OFFSET);

	let value: Word = if src_reg == dest_reg
	{
		instruction.bits(MASK_LITERAL, LITERAL_OFFSET, LITERAL_OFFSET)
	}
	else
	{
		registers.get_register_value(src_reg)
	};

	if src_reg != dest_reg && value != 0
	{
		return InstructionResult {
			fault: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	// Don't allow assigning to PC via an instruction literal,
	// since the range of the literal is too small to be useful.
	// We return a RES fault here since this combination of bits
	// could potentially be useful in the future for some other
	// operation, so we don't want to allow people to rely
	// on it for anything right now.
	if src_reg == dest_reg && dest_reg == RegisterIndex::Pc
	{
		return InstructionResult {
			fault: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	return InstructionResult {
		sr: Some(StatusRegisterFlag::Z.set_if(0, value == 0)),
		..Default::default()
	}
	.set_register(dest_reg, value);
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
			fault: Some(REG_VAL_FAULT_RES),
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
		Some(REG_VAL_FAULT_RES)
	}
	else
	{
		None
	};
}
