use super::arch::*;
use super::cpu::Registers;

pub fn execute(instruction: Word, registers: Registers) -> Registers
{
	let index: usize = OpCode::from_word(instruction).to_value() as usize;
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
	executeUnassigned, // 0x00
	executeUnassigned, // 0x01
	executeUnassigned, // 0x02
	executeUnassigned, // 0x03
	executeUnassigned, // 0x04
	executeUnassigned, // 0x05
	executeUnassigned, // 0x06
	executeUnassigned, // 0x07
	executeUnassigned, // 0x08
	executeUnassigned, // 0x09
	executeUnassigned, // 0x0A
	executeUnassigned, // 0x0B
	executeUnassigned, // 0x0C
	executeUnassigned, // 0x0D
	executeUnassigned, // 0x0E
	executeUnassigned, // 0x0F
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
		fault: Some(FaultCode::Ini.to_word(0)),
		..Default::default()
	};
}
