use super::arch::*;
use super::cpu::Registers;

pub fn execute(instruction: Word, registers: Registers) -> Registers
{
	let index: usize = OpCode::from_word(instruction).as_value() as usize;
	debug_assert!(index < NUM_OPCODES);

	return INSTRUCTION_CALLBACKS[index](instruction, &registers).apply(registers);
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

type InstructionCallback = fn(instruction: Word, registers: &Registers) -> InstructionResult;

static INSTRUCTION_CALLBACKS: [InstructionCallback; NUM_OPCODES] = [
	executeNop,        // 0x00 Nop
	executeUnassigned, // 0x01 Add
	executeUnassigned, // 0x02 Sub
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

fn executeUnassigned(_: Word, _: &Registers) -> InstructionResult
{
	return InstructionResult {
		fault: Some(FaultCode::Ini.as_word(0)),
		..Default::default()
	};
}

fn executeNop(instruction: Word, _: &Registers) -> InstructionResult
{
	return InstructionResult {
		// Fault if any of the arg bits are set
		fault: faultRegisterIfReservedBitsSet(instruction, INSTRUCTION_ARG_MASK),
		..Default::default()
	};
}

fn faultRegisterIfReservedBitsSet(instruction: Word, reserved_mask: u16) -> Option<Word>
{
	return if instruction.any_bits_set(reserved_mask)
	{
		Some(FaultCode::Res.as_word(0))
	}
	else
	{
		None
	};
}
