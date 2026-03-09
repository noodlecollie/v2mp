mod instructions;
mod signals;

use crate::arch::*;
use crate::cpu::signals::SignalRegisters;

pub struct Cpu
{
	pub registers: Registers,
}

impl Cpu
{
	pub fn new() -> Self
	{
		return Self {
			registers: Registers::default(),
		};
	}
}

pub struct Registers
{
	pub pc: Word,
	pub sr: Word,
	pub lr: Word,
	pub r0: Word,
	pub r1: Word,
	pub ir: Word,
	pub sp: Word,
	pub fr: Word,
	pub s0: Word,
	pub s1: Word,
	pub s2: Word,
}

impl Registers
{
	pub fn get_register_value(&self, index: RegisterIndex) -> Word
	{
		return match index
		{
			RegisterIndex::R0 => self.r0,
			RegisterIndex::R1 => self.r1,
			RegisterIndex::Lr => self.lr,
			RegisterIndex::Pc => self.pc,
		};
	}
}

impl Default for Registers
{
	fn default() -> Self
	{
		return Self {
			pc: 0,
			sr: 0,
			lr: 0,
			r0: 0,
			r1: 0,
			ir: 0,
			sp: 0,
			fr: 0,
			s0: 0,
			s1: 0,
			s2: 0,
		};
	}
}

#[derive(Debug)]
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

	pub fn set_signal_registers(mut self, registers: SignalRegisters) -> Self
	{
		self.s0 = Some(registers.s0);
		self.s1 = Some(registers.s1);
		self.s2 = Some(registers.s2);
		self.fr = Some(registers.fr);

		return self;
	}

	pub fn apply(self, mut registers: Registers) -> Registers
	{
		const REG_VAL_FAULT_NONE_WITH_INVALID_ARGS: Word =
			FaultCode::Div.as_register_value(FAULT_ARGS_RESERVED_BITS_SET);

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

		// If there is no fault code set, but there are fault bits set,
		// map this to a Res fault.
		let fault_bits: WordBits = WordBits(registers.fr);

		if FaultCode::from_word_bits(fault_bits) == FaultCode::None
			&& fault_bits.any_bits_set(FAULT_ARG_MASK)
		{
			registers.fr = REG_VAL_FAULT_NONE_WITH_INVALID_ARGS;
		}

		return registers;
	}
}
