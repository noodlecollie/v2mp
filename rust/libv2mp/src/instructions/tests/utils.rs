use crate::arch::*;
use crate::execution_context::Registers;
use crate::instructions::execute;

pub struct Equation<ResType>
{
	pub lhs: Word,
	pub rhs: Word,
	pub result: ResType,
	pub overflow: bool,

	op_symbol: &'static str,
}

impl Equation<Word>
{
	pub const fn new_add(lhs: Word, rhs: Word, result: Word, overflow: bool) -> Self
	{
		return Self {
			lhs,
			rhs,
			result,
			overflow,

			op_symbol: "+",
		};
	}

	pub const fn new_addl(lhs: Word, rhs: Byte, result: Word, overflow: bool) -> Self
	{
		return Self {
			lhs,
			rhs: rhs as Word,
			result,
			overflow,

			op_symbol: "+",
		};
	}

	pub const fn new_sub(lhs: Word, rhs: Word, result: Word, underflow: bool) -> Self
	{
		return Self {
			lhs,
			rhs,
			result,
			overflow: underflow,

			op_symbol: "-",
		};
	}

	pub const fn new_subl(lhs: Word, rhs: Byte, result: Word, underflow: bool) -> Self
	{
		return Self {
			lhs,
			rhs: rhs as Word,
			result,
			overflow: underflow,

			op_symbol: "-",
		};
	}

	pub fn description(&self, source_reg: RegisterIndex, dest_reg: RegisterIndex) -> String
	{
		let overflow_str: &str = if self.op_symbol == "+"
		{
			"overflow"
		}
		else
		{
			"underflow"
		};

		return format!(
			"{:#06x} ({}) {} {:#06x} ({}) = {:#06x} ({}{overflow_str})",
			self.lhs,
			dest_reg.name(),
			self.op_symbol,
			self.rhs,
			source_reg.name(),
			self.result,
			if self.overflow { "" } else { "no " }
		);
	}
}

impl Equation<u32>
{
	pub const fn new_mulr(lhs: Word, rhs: Word, result: u32, overflow: bool) -> Self
	{
		return Self {
			lhs,
			rhs,
			result,
			overflow,

			op_symbol: "*",
		};
	}

	pub const fn new_mull(lhs: Word, rhs: Byte, result: u32, overflow: bool) -> Self
	{
		return Self {
			lhs,
			rhs: rhs as Word,
			result,
			overflow,

			op_symbol: "*",
		};
	}

	pub fn description(&self, source_reg: Option<RegisterIndex>, dest_reg: RegisterIndex)
	-> String
	{
		let can_overflow: bool = self.op_symbol == "*";
		let source_name: &str = source_reg.map(|reg| reg.name()).unwrap_or("literal");

		if can_overflow
		{
			return format!(
				"{:#06x} ({}) {} {:#06x} ({}) = {:#010x} ({}overflow)",
				self.lhs,
				dest_reg.name(),
				self.op_symbol,
				self.rhs,
				source_name,
				self.result,
				if self.overflow { "" } else { "no " }
			);
		}
		else
		{
			return format!(
				"{:#06x} ({}) {} {:#06x} ({}) = {:#010x}",
				self.lhs,
				dest_reg.name(),
				self.op_symbol,
				self.rhs,
				source_name,
				self.result
			);
		}
	}
}

pub type SingleEquation = Equation<Word>;
pub type DoubleEquation = Equation<u32>;

pub fn assert_register(equation: &str, name: &str, expected: Word, actual: Word)
{
	assert_eq!(
		expected, actual,
		"Equation \"{equation}\": Expected register {name} to be {:#06x}, but it was {:#06x}",
		expected, actual
	);
}

pub fn execute_and_check_fault(name: &str, registers: Registers, fault: Word)
{
	let mut post: Registers = execute(registers.clone());

	assert_eq!(
		fault, post.fr,
		"{name}: Expected register fr to be {:#06x}, but it was {:#06x}",
		fault, post.fr
	);

	// Reset fault register, and ensure that the rest of the registers match.
	post.fr = registers.fr;
	assert_eq!(registers, post, "{name}: Registers mismatched");
}
