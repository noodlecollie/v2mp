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
			pc: Word(0),
			sr: Word(0),
			lr: Word(0),
			r0: Word(0),
			r1: Word(0),
			ir: Word(0),
			sp: Word(0),
			fault: Word(0),
		};
	}
}
