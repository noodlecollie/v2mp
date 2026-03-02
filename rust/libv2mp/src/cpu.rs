use crate::arch::*;

pub struct Cpu
{
	pub registers: Registers,
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
	pub fault: Word,
}

impl Cpu
{
	pub fn new() -> Self
	{
		return Self {
			registers: Registers::new(),
		};
	}
}

impl Registers
{
	pub fn new() -> Self
	{
		return Self {
			pc: 0,
			sr: 0,
			lr: 0,
			r0: 0,
			r1: 0,
			ir: 0,
			sp: 0,
			fault: 0,
		};
	}

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
