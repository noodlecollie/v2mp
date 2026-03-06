mod instructions;

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
	pub fr: Word,
	pub s0: Word,
	pub s1: Word,
	pub s2: Word,
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
