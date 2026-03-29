use crate::arch::*;

pub struct Equation
{
	pub lhs: Word,
	pub rhs: Word,
	pub result: Word,
	pub overflow: bool,

	op_symbol: &'static str,
}

impl Equation
{
	pub const fn new_add(lhs: Word, rhs: Word, result: Word, overflow: bool) -> Equation
	{
		return Equation {
			lhs,
			rhs,
			result,
			overflow,

			op_symbol: "+",
		};
	}

	pub const fn new_addl(lhs: Word, rhs: Byte, result: Word, overflow: bool) -> Equation
	{
		return Equation {
			lhs,
			rhs: rhs as Word,
			result,
			overflow,

			op_symbol: "+",
		};
	}

	pub const fn new_sub(lhs: Word, rhs: Word, result: Word, underflow: bool) -> Equation
	{
		return Equation {
			lhs,
			rhs,
			result,
			overflow: underflow,

			op_symbol: "-",
		};
	}

	pub const fn new_subl(lhs: Word, rhs: Byte, result: Word, underflow: bool) -> Equation
	{
		return Equation {
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

pub fn assert_register(equation: &str, name: &str, expected: Word, actual: Word)
{
	assert_eq!(
		expected, actual,
		"Equation \"{equation}\": Expected register {name} to be {:#06x}, but it was {:#06x}",
		expected, actual
	);
}
