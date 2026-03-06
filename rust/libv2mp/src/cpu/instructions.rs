use crate::arch::*;
use crate::cpu::Registers;

pub fn execute(registers: Registers) -> Registers
{
	let instruction: WordBits = WordBits(registers.ir);

	let index: usize = OpCode::from_word_bits(instruction).as_value() as usize;
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
	pub fr: Option<Word>,
	pub s0: Option<Word>,
	pub s1: Option<Word>,
	pub s2: Option<Word>,
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
	pub fn get(instruction: WordBits, registers: &Registers) -> Self
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

type InstructionCallback = fn(instruction: WordBits, registers: &Registers) -> InstructionResult;

static INSTRUCTION_CALLBACKS: [InstructionCallback; NUM_OPCODES] = [
	executeNop,        // 0x00 Nop
	executeAdd,        // 0x01 Add
	executeSub,        // 0x02 Sub
	executeMul,        // 0x03 Mul
	executeDiv,        // 0x04 Div
	executeAsgn,       // 0x05 Asgn
	executeShft,       // 0x06 Shft
	executeBitw,       // 0x07 Bitw
	executeCbx,        // 0x08 Cbx
	executeLdst,       // 0x09 Ldst
	executeStk,        // 0x0A Stk
	executeSig,        // 0x0B Sig
	executeUnassigned, // 0x0C Unassigned0
	executeUnassigned, // 0x0D Unassigned1
	executeUnassigned, // 0x0E Unassigned2
	executeUnassigned, // 0x0F Unassigned3
];

impl InstructionResult
{
	pub fn apply(self, mut registers: Registers) -> Registers
	{
		registers = Registers {
			pc: self.pc.unwrap_or(registers.pc),
			sr: self.sr.unwrap_or(registers.sr),
			lr: self.lr.unwrap_or(registers.lr),
			r0: self.r0.unwrap_or(registers.r0),
			r1: self.r1.unwrap_or(registers.r1),
			ir: self.ir.unwrap_or(registers.ir),
			sp: self.sp.unwrap_or(registers.sp),
			fr: self.fr.unwrap_or(registers.fr),
			s0: self.fr.unwrap_or(SignalCode::None.as_value()),
			s1: self.fr.unwrap_or(0),
			s2: self.fr.unwrap_or(0),
		};

		{
			// If there is no fault code set, but there are fault bits set,
			// map this to a Res fault.
			let fault_bits: WordBits = WordBits(registers.fr);

			if FaultCode::from_word_bits(fault_bits) == FaultCode::None
				&& fault_bits.any_bits_set(FAULT_ARG_MASK)
			{
				registers.fr = FaultCode::Res.as_register_value(FAULT_ARGS_RESERVED_BITS_SET);
			}
		}

		return registers;
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
			fr: None,
			s0: None,
			s1: None,
			s2: None,
		};
	}
}

fn executeUnassigned(_: WordBits, _: &Registers) -> InstructionResult
{
	return InstructionResult {
		fr: Some(FaultCode::Ini.as_register_value(0)),
		..Default::default()
	};
}

fn executeNop(instruction: WordBits, _: &Registers) -> InstructionResult
{
	return InstructionResult {
		// Fault if any of the arg bits are set
		fr: faultRegisterIfReservedBitsSet(instruction, INSTRUCTION_ARG_MASK),
		..Default::default()
	};
}

fn executeAdd(instruction: WordBits, registers: &Registers) -> InstructionResult
{
	return executeAddOrSub(instruction, registers, AddOrSub::Add);
}

fn executeSub(instruction: WordBits, registers: &Registers) -> InstructionResult
{
	return executeAddOrSub(instruction, registers, AddOrSub::Add);
}

fn executeMul(instruction: WordBits, registers: &Registers) -> InstructionResult
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
			fr: Some(REG_VAL_FAULT_RES),
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

fn executeDiv(instruction: WordBits, registers: &Registers) -> InstructionResult
{
	const MASK_RESBITS: Word = 0x0100;

	if instruction.any_bits_set(MASK_RESBITS)
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	let params: MulDivParams = MulDivParams::get(instruction, registers);

	if params.source_value == 0
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_DIV),
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

fn executeAsgn(instruction: WordBits, registers: &Registers) -> InstructionResult
{
	const SRC_REG_IDX_OFFSET: u8 = 10;
	const DEST_REG_IDX_OFFSET: u8 = 8;
	const MASK_LITERAL: Word = 0x00FF;
	const LITERAL_OFFSET: u8 = 0;

	let src_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, SRC_REG_IDX_OFFSET);
	let dest_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, DEST_REG_IDX_OFFSET);

	let value: Word = if src_reg == dest_reg
	{
		instruction.bits(MASK_LITERAL, LITERAL_OFFSET)
	}
	else
	{
		registers.get_register_value(src_reg)
	};

	if src_reg != dest_reg && value != 0
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
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
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	return InstructionResult {
		sr: Some(StatusRegisterFlag::Z.set_if(0, value == 0)),
		..Default::default()
	}
	.set_register(dest_reg, value);
}

fn executeShft(instruction: WordBits, registers: &Registers) -> InstructionResult
{
	const MASK_RESBITS: Word = 0x00E0;
	const SRC_REG_IDX_OFFSET: u8 = 10;
	const DEST_REG_IDX_OFFSET: u8 = 8;
	const MASK_LITERAL: Word = 0x001F;
	const LITERAL_OFFSET: u8 = 0;

	if instruction.any_bits_set(MASK_RESBITS)
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	let src_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, SRC_REG_IDX_OFFSET);
	let dest_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, DEST_REG_IDX_OFFSET);

	let raw_shift_arg: Word = if src_reg == dest_reg
	{
		instruction.bits(MASK_LITERAL, LITERAL_OFFSET)
	}
	else
	{
		registers.get_register_value(src_reg) & MASK_LITERAL
	};

	if src_reg == dest_reg && raw_shift_arg != 0
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	// To convert to signed: add 1s in bits above our masked value.
	let signed_shift_value: i8 = (raw_shift_arg | (!MASK_LITERAL)) as i8;
	let shift_magnitude: u8 = signed_shift_value.abs() as u8;

	let original_value: Word = registers.get_register_value(dest_reg);

	let shifted_value: Word = if signed_shift_value > 0
	{
		original_value << shift_magnitude
	}
	else
	{
		original_value >> shift_magnitude
	};

	// To check whether any '1' bits got shifted off the end,
	// we can mask the original value with the span of bits
	// that *did* get shifted off the end. If the intersection
	// of these two values is not zero, bits in the register
	// were shifted off the end.

	// This mask represents the locations of bits in the original
	// value that ended up being shifted out of the register.
	// For example, with a shift of +4, 0xFFFF gets shifted up
	// by 12 bits to become 0xF000. The most significant bits here
	// are the bits that were shifted out of the register.
	let check_mask: Word = if signed_shift_value > 0
	{
		0xFFFFu16 << (16 - shift_magnitude)
	}
	else
	{
		0xFFFFu16 >> (16 - shift_magnitude)
	};

	let shifted_off_end: bool = (original_value & check_mask) != 0;

	let status_result: Word = 0;
	let status_result: Word = StatusRegisterFlag::C.set_if(status_result, shifted_off_end);
	let status_result: Word = StatusRegisterFlag::Z.set_if(status_result, shifted_value == 0);

	return InstructionResult {
		sr: Some(status_result),
		..Default::default()
	}
	.set_register(dest_reg, shifted_value);
}

fn executeBitw(instruction: WordBits, registers: &Registers) -> InstructionResult
{
	const MASK_RESBITS: Word = 0x0010;
	const MASK_FLIP: Word = 0x0020;
	const FLIP_MASK_OFFSET: u8 = 5;
	const MASK_SHIFT: Word = 0x000F;
	const SHIFT_MASK_OFFSET: u8 = 0;
	const SRC_REG_IDX_OFFSET: u8 = 10;
	const DEST_REG_IDX_OFFSET: u8 = 8;
	const BITWISE_OP_OFFSET: u8 = 6;

	if instruction.any_bits_set(MASK_RESBITS)
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	let src_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, SRC_REG_IDX_OFFSET);
	let dest_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, DEST_REG_IDX_OFFSET);
	let bitwise_op: BitwiseOp = BitwiseOp::from_word_bits(instruction, BITWISE_OP_OFFSET);
	let shift: Word = instruction.bits(MASK_SHIFT, SHIFT_MASK_OFFSET);
	let flip: bool = instruction.bits(MASK_FLIP, FLIP_MASK_OFFSET) != 0;
	let shift_or_flip: bool = shift != 0 || flip;

	let reserved_bits_set: bool = (src_reg != dest_reg && shift_or_flip)
		|| (src_reg == dest_reg && bitwise_op == BitwiseOp::Not && shift_or_flip);

	if reserved_bits_set
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	// A lambda which we can call only if we need the mask.
	let get_bitmask = || {
		if src_reg != dest_reg
		{
			registers.get_register_value(src_reg)
		}
		else if flip
		{
			!(1u16 << shift)
		}
		else
		{
			1u16 << shift
		}
	};

	let dest_reg_value: Word = registers.get_register_value(dest_reg);

	let op_result: Word = match bitwise_op
	{
		BitwiseOp::And => dest_reg_value & get_bitmask(),
		BitwiseOp::Or => dest_reg_value | get_bitmask(),
		BitwiseOp::Xor => dest_reg_value ^ get_bitmask(),
		BitwiseOp::Not => !dest_reg_value,
	};

	let status_result: Word = 0;
	let status_result: Word = StatusRegisterFlag::Z.set_if(status_result, op_result == 0);

	return InstructionResult {
		sr: Some(status_result),
		..Default::default()
	}
	.set_register(dest_reg, op_result);
}

fn executeCbx(instruction: WordBits, registers: &Registers) -> InstructionResult
{
	const MASK_RESBITS: Word = 0x0300;
	const MASK_LR_IS_TARGET: Word = 0x0800;
	const MASK_BRANCH_ON_SR_Z: Word = 0x0400;
	const MASK_LITERAL: Word = 0x00FF;
	const LR_IS_TARGET_OFFSET: u8 = 11;
	const BRANCH_ON_SR_Z_OFFSET: u8 = 10;
	const LITERAL_OFFSET: u8 = 0;

	if instruction.any_bits_set(MASK_RESBITS)
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	let lr_is_target: bool = instruction.bits(MASK_LR_IS_TARGET, LR_IS_TARGET_OFFSET) != 0;
	let literal_offset: u8 = instruction.bits(MASK_LITERAL, LITERAL_OFFSET) as u8;

	if lr_is_target && literal_offset != 0
	{
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
			..Default::default()
		};
	}

	let branch_on_sr_z: bool = instruction.bits(MASK_BRANCH_ON_SR_Z, BRANCH_ON_SR_Z_OFFSET) != 0;
	let branch_on_sr_c: bool = !branch_on_sr_z;
	let should_branch: bool = (branch_on_sr_z && StatusRegisterFlag::Z.is_set(registers.sr))
		|| (branch_on_sr_c && StatusRegisterFlag::C.is_set(registers.sr));

	if should_branch
	{
		if lr_is_target
		{
			return InstructionResult {
				sr: Some(0),
				pc: Some(registers.lr),
				..Default::default()
			};
		}
		else
		{
			let pc_offset: Word = ((literal_offset) as usize * SIZE_OF_WORD) as Word;
			let pc: Word = registers.pc.wrapping_add(pc_offset);

			return InstructionResult {
				sr: Some(0),
				pc: Some(pc),
				..Default::default()
			};
		}
	}

	return InstructionResult {
		sr: Some(StatusRegisterFlag::Z.set(0)),
		..Default::default()
	};
}

fn executeLdst(_: WordBits, _: &Registers) -> InstructionResult
{
	todo!();
}

fn executeStk(_: WordBits, _: &Registers) -> InstructionResult
{
	todo!();
}

fn executeSig(_: WordBits, _: &Registers) -> InstructionResult
{
	todo!();
}

fn executeAddOrSub(
	instruction: WordBits,
	registers: &Registers,
	operation: AddOrSub,
) -> InstructionResult
{
	const SRC_REG_IDX_OFFSET: u8 = 10;
	const DEST_REG_IDX_OFFSET: u8 = 8;
	const LITERAL_MASK: Word = 0x00FF;
	const LITERAL_OFFSET: u8 = 0;

	let src_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, SRC_REG_IDX_OFFSET);
	let dest_reg: RegisterIndex = RegisterIndex::from_word_bits(instruction, DEST_REG_IDX_OFFSET);
	let literal: Word = instruction.bits(LITERAL_MASK, LITERAL_OFFSET);

	if src_reg != dest_reg && literal != 0
	{
		// Should not have a literal value here, since
		// the source register is being used as a value.
		return InstructionResult {
			fr: Some(REG_VAL_FAULT_RES),
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

fn faultRegisterIfReservedBitsSet(instruction: WordBits, reserved_mask: Word) -> Option<Word>
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
