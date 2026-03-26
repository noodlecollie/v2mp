use crate::arch::*;
use crate::execution_context::Registers;
use crate::instructions::execute;
use crate::instructions::tests::asm;

struct RegValues
{
	pub r0: Word,
	pub r1: Word,
	pub lr: Word,
	pub pc: Word,
}

impl RegValues
{
	pub const fn get(&self, register: RegisterIndex) -> Word
	{
		return match register
		{
			RegisterIndex::R0 => self.r0,
			RegisterIndex::R1 => self.r1,
			RegisterIndex::Lr => self.lr,
			RegisterIndex::Pc => self.pc,
		};
	}
}

struct AddRegisters
{
	pub ir: Word,
	pub r0: Word,
	pub r1: Word,
	pub lr: Word,
	pub pc: Word,
}

impl AddRegisters
{
	pub const fn new(instruction: Word, values: &RegValues) -> AddRegisters
	{
		return AddRegisters {
			ir: instruction,
			r0: values.r0,
			r1: values.r1,
			lr: values.lr,
			pc: values.pc,
		};
	}

	pub fn as_registers(&self) -> Registers
	{
		return Registers {
			r0: self.r0,
			r1: self.r1,
			lr: self.lr,
			pc: self.pc,
			ir: self.ir,
			..Default::default()
		};
	}

	pub fn add(&mut self, register: RegisterIndex, value: Word)
	{
		match register
		{
			RegisterIndex::R0 => self.r0 = self.r0.wrapping_add(value),
			RegisterIndex::R1 => self.r1 = self.r1.wrapping_add(value),
			RegisterIndex::Lr => self.lr = self.lr.wrapping_add(value),
			RegisterIndex::Pc =>
			{
				let to_add: usize = value as usize * SIZE_OF_WORD as usize;
				self.pc = (self.pc as usize + to_add) as Word;
			}
		}
	}

	pub fn subtract(&mut self, register: RegisterIndex, value: Word)
	{
		match register
		{
			RegisterIndex::R0 => self.r0 = self.r0.wrapping_sub(value),
			RegisterIndex::R1 => self.r1 = self.r1.wrapping_sub(value),
			RegisterIndex::Lr => self.lr = self.lr.wrapping_sub(value),
			RegisterIndex::Pc => self.pc = self.pc.wrapping_sub(value),
		}
	}

	pub fn assert_registers(&self, registers: &Registers)
	{
		AddRegisters::assert_register("ir", self.ir, registers.ir);
		AddRegisters::assert_register("r0", self.r0, registers.r0);
		AddRegisters::assert_register("r1", self.r1, registers.r1);
		AddRegisters::assert_register("lr", self.lr, registers.lr);
		AddRegisters::assert_register("pc", self.pc, registers.pc);
		AddRegisters::assert_register("fr", 0x0000, registers.fr);
		AddRegisters::assert_register("sp", 0x0000, registers.sp);
		AddRegisters::assert_register("s0", 0x0000, registers.s0);
		AddRegisters::assert_register("s1", 0x0000, registers.s1);
		AddRegisters::assert_register("s2", 0x0000, registers.s2);
	}

	fn assert_register(name: &str, expected: Word, actual: Word)
	{
		assert_eq!(
			expected, actual,
			"Expected {name} to be {:#06x}, but it was {:#06x}",
			expected, actual
		);
	}
}

// TODO: Check status register too
fn add_and_check(init_values: &RegValues, source: RegisterIndex, dest: RegisterIndex)
{
	let instruction: Word = asm::addr(source, dest);
	let mut spec: AddRegisters = AddRegisters::new(instruction, init_values);
	let registers: Registers = execute(spec.as_registers());

	spec.add(dest, init_values.get(source));
	spec.assert_registers(&registers);
}

const SMALL_VALUES: RegValues = RegValues {
	r0: 0x0001,
	r1: 0x0002,
	lr: 0x0003,
	pc: 0x0004,
};

const ARBITRARY_VALUES: RegValues = RegValues {
	r0: 0x0101,
	r1: 0xBEEF,
	lr: 0xB00B,
	pc: 0x1234,
};

const MAXIMAL_VALUES: RegValues = RegValues {
	r0: 0xFFFF,
	r1: 0xFFFF,
	lr: 0xFFFF,
	pc: 0xFFFE,
};

const SOME_ZERO_VALUES: RegValues = RegValues {
	r0: 0x0000,
	r1: 0x0001,
	lr: 0x0000,
	pc: 0x0002,
};

const ALL_ZERO_VALUES: RegValues = RegValues {
	r0: 0x0000,
	r1: 0x0000,
	lr: 0x0000,
	pc: 0x0000,
};

const ALL_VALUE_SETS: [RegValues; 5] = [
	SMALL_VALUES,
	ARBITRARY_VALUES,
	MAXIMAL_VALUES,
	SOME_ZERO_VALUES,
	ALL_ZERO_VALUES,
];

#[test]
fn add_r1_to_r0()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::R1, RegisterIndex::R0);
	}
}

#[test]
fn add_lr_to_r0()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::Lr, RegisterIndex::R0);
	}
}

#[test]
fn add_pc_to_r0()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::Pc, RegisterIndex::R0);
	}
}

#[test]
fn add_r0_to_r1()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::R0, RegisterIndex::R1);
	}
}

#[test]
fn add_lr_to_r1()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::Lr, RegisterIndex::R1);
	}
}

#[test]
fn add_pc_to_r1()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::Pc, RegisterIndex::R1);
	}
}

#[test]
fn add_r0_to_lr()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::R0, RegisterIndex::Lr);
	}
}

#[test]
fn add_r1_to_lr()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::R1, RegisterIndex::Lr);
	}
}

#[test]
fn add_pc_to_lr()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::Pc, RegisterIndex::Lr);
	}
}

#[test]
fn add_r0_to_pc()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::R0, RegisterIndex::Pc);
	}
}

#[test]
fn add_r1_to_pc()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::R1, RegisterIndex::Pc);
	}
}

#[test]
fn add_lr_to_pc()
{
	for values in ALL_VALUE_SETS
	{
		add_and_check(&values, RegisterIndex::Lr, RegisterIndex::Pc);
	}
}
