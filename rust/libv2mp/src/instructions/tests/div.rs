use super::utils::{DoubleEquation, assert_register, execute_and_check_fault};
use crate::arch::*;
use crate::execution_context::Registers;
use crate::instructions::execute;
use crate::instructions::tests::asm;

#[test]
fn happy_div_register_unsigned()
{
	// a / b = c rem d
	const EQUATIONS: [DoubleEquation; 1] = [
		// Simple small values
		DoubleEquation::new_divr(0x0002, 0x0002, 0x0001, 0x0000),
	];

	check_equations_for_non_pc_registers(&EQUATIONS, CheckOperation::Register(asm::divru));
}

enum CheckOperation
{
	Register(fn(RegisterIndex) -> Word),
	Literal(fn(RegisterIndex, Byte) -> Word),
}

fn check_equations_for_non_pc_registers(equations: &[DoubleEquation], operation: CheckOperation)
{
	for equation in equations
	{
		for dest in [RegisterIndex::R0, RegisterIndex::R1]
		{
			let source: Option<RegisterIndex> = match operation
			{
				CheckOperation::Register(_) =>
				{
					if dest == RegisterIndex::R0
					{
						Some(RegisterIndex::R1)
					}
					else
					{
						Some(RegisterIndex::R0)
					}
				}
				CheckOperation::Literal(_) => None,
			};

			let equation_str: String = equation.description(source, dest);
			let mut pre: Registers = Registers::default();

			pre.ir = match operation
			{
				CheckOperation::Literal(func) => func(dest, equation.rhs as Byte),
				CheckOperation::Register(func) => func(dest),
			};

			source.map(|reg| match reg
			{
				RegisterIndex::R0 => pre.r0 = equation.rhs,
				RegisterIndex::R1 => pre.r1 = equation.rhs,
				_ => unreachable!(),
			});

			match dest
			{
				RegisterIndex::R0 => pre.r0 = equation.lhs,
				RegisterIndex::R1 => pre.r1 = equation.lhs,
				_ => unreachable!(),
			};

			let post: Registers = execute(pre.clone());

			assert_register(&equation_str, "ir", pre.ir, post.ir);

			source.map(|reg| match reg
			{
				RegisterIndex::R0 => assert_register(&equation_str, "r0", equation.rhs, post.r0),
				RegisterIndex::R1 => assert_register(&equation_str, "r1", equation.rhs, post.r1),
				_ => unreachable!(),
			});

			match dest
			{
				RegisterIndex::R0 => assert_register(
					&equation_str,
					"r0",
					(equation.result & 0x0000FFFF) as Word,
					post.r0,
				),
				RegisterIndex::R1 => assert_register(
					&equation_str,
					"r1",
					(equation.result & 0x0000FFFF) as Word,
					post.r1,
				),
				_ => unreachable!(),
			};

			assert_register(
				&equation_str,
				"lr",
				((equation.result & 0xFFFF0000) >> 16) as Word,
				post.lr,
			);

			assert_register(&equation_str, "pc", pre.pc, post.pc);

			let expected_sr: Word = 0x0000;
			let expected_sr: Word =
				StatusRegisterFlag::C.set_if(expected_sr, (equation.result & 0xFFFF0000) != 0);
			let expected_sr: Word =
				StatusRegisterFlag::Z.set_if(expected_sr, (equation.result & 0x0000FFFF) == 0);

			assert_register(&equation_str, "fr", 0x0000, post.fr);
			assert_register(&equation_str, "sp", 0x0000, post.sp);
			assert_register(&equation_str, "s0", 0x0000, post.s0);
			assert_register(&equation_str, "s1", 0x0000, post.s1);
			assert_register(&equation_str, "s2", 0x0000, post.s2);
			assert_register(&equation_str, "sr", expected_sr, post.sr);
		}
	}
}
