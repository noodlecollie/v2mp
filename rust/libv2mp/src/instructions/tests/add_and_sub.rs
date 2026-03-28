use super::utils::{Equation, assert_register};
use crate::arch::*;
use crate::execution_context::Registers;
use crate::instructions::execute;
use crate::instructions::tests::asm;

#[test]
fn happy_add_to_non_pc_register()
{
	// a + b = c (true if overflow)
	const EQUATIONS: [Equation; 8] = [
		// Simple small values
		Equation::new_add(0x0001, 0x0001, 0x0002, false),
		Equation::new_add(0x1000, 0x0001, 0x1001, false),
		// Overflow
		Equation::new_add(0xFFFF, 0x0001, 0x0000, true),
		Equation::new_add(0x0001, 0xFFFF, 0x0000, true),
		Equation::new_add(0xFFFF, 0xFFFF, 0xFFFE, true),
		// Zero
		Equation::new_add(0x1234, 0x0000, 0x1234, false),
		Equation::new_add(0x0000, 0x5678, 0x5678, false),
		Equation::new_add(0x0000, 0x0000, 0x0000, false),
	];

	check_equations_for_non_pc_registers(&EQUATIONS, asm::addr);
}

#[test]
fn happy_add_to_pc_register()
{
	// a + b = c (true if overflow)
	const EQUATIONS: [Equation; 8] = [
		// Simple small values
		Equation::new_add(0x0010, 0x0001, 0x0012, false),
		Equation::new_add(0x1000, 0x0001, 0x1002, false),
		// Overflow
		Equation::new_add(0xFFFE, 0x0001, 0x0000, true),
		Equation::new_add(0x0000, 0xFFFF, 0xFFFE, true),
		Equation::new_add(0xFFFE, 0xFFFF, 0xFFFC, true),
		// Zero
		Equation::new_add(0x1234, 0x0000, 0x1234, false),
		Equation::new_add(0x0000, 0x0020, 0x0040, false),
		Equation::new_add(0x0000, 0x0000, 0x0000, false),
	];

	check_equations_for_pc_register(&EQUATIONS, asm::addr);
}

#[test]
fn happy_subtract_to_non_pc_register()
{
	// a - b = c (true if underflow)
	const EQUATIONS: [Equation; 8] = [
		// Simple small values
		Equation::new_sub(0x0005, 0x0003, 0x0002, false),
		Equation::new_sub(0x1010, 0x1000, 0x0010, false),
		// Underflow
		Equation::new_sub(0x0002, 0x0004, 0xFFFE, true),
		Equation::new_sub(0xFFFE, 0xFFFF, 0xFFFF, true),
		Equation::new_sub(0x0000, 0xFFFF, 0x0001, true),
		// Zero
		Equation::new_sub(0x1234, 0x0000, 0x1234, false),
		Equation::new_sub(0x5678, 0x5678, 0x0000, false),
		Equation::new_sub(0x0000, 0x0000, 0x0000, false),
	];

	check_equations_for_non_pc_registers(&EQUATIONS, asm::subr);
}

#[test]
fn happy_subtract_from_pc_register()
{
	// a - b = c (true if underflow)
	const EQUATIONS: [Equation; 9] = [
		// Simple small values
		Equation::new_sub(0x0010, 0x0001, 0x000E, false),
		Equation::new_sub(0x1000, 0x0002, 0x0FFC, false),
		// Underflow
		Equation::new_sub(0x0002, 0x0004, 0xFFFA, true),
		Equation::new_sub(0xFFFF, 0xFFFF, 0x0001, true),
		Equation::new_sub(0x0000, 0xFFFF, 0x0002, true),
		// Zero
		Equation::new_sub(0x1234, 0x0000, 0x1234, false),
		Equation::new_sub(0x0000, 0x0020, 0xFFC0, true),
		Equation::new_sub(0x0000, 0x0000, 0x0000, false),
		Equation::new_sub(0xFFFE, 0xFFFF, 0x0000, true),
	];

	check_equations_for_pc_register(&EQUATIONS, asm::subr);
}

#[test]
fn happy_add_literal_to_non_pc_register()
{
	// a + b = c (true if overflow)
	const EQUATIONS: [Equation; 8] = [
		// Simple small values
		Equation::new_add(0x0001, 0x01, 0x0002, false),
		Equation::new_add(0x1000, 0x01, 0x1001, false),
		// Overflow
		Equation::new_add(0xFFFF, 0x01, 0x0000, true),
		Equation::new_add(0xFF01, 0xFF, 0x0000, true),
		Equation::new_add(0xFFFF, 0xFF, 0x00FE, true),
		// Zero
		Equation::new_add(0x1234, 0x00, 0x1234, false),
		Equation::new_add(0x0000, 0x78, 0x0078, false),
		Equation::new_add(0x0000, 0x00, 0x0000, false),
	];

	check_literal_op_for_non_pc_registers(&EQUATIONS, asm::addl);
}

// TODO:
// - Literal adds
// - Literal subtractions
// - Faults

fn check_equations_for_non_pc_registers<Op>(equations: &[Equation], operation: Op)
where
	Op: Fn(RegisterIndex, RegisterIndex) -> Word,
{
	for equation in equations
	{
		for source in [
			RegisterIndex::R0,
			RegisterIndex::R1,
			RegisterIndex::Lr,
			RegisterIndex::Pc,
		]
		{
			for dest in [RegisterIndex::R0, RegisterIndex::R1, RegisterIndex::Lr]
			{
				if source == dest
				{
					// This would imply a literal addition from the instruction word, which is not
					// what we're doing here.
					continue;
				}

				let equation_str: String = equation.description(source, dest);
				let mut pre: Registers = Registers::default();

				pre.ir = operation(source, dest);

				match dest
				{
					RegisterIndex::R0 => pre.r0 = equation.lhs,
					RegisterIndex::R1 => pre.r1 = equation.lhs,
					RegisterIndex::Lr => pre.lr = equation.lhs,
					RegisterIndex::Pc => unreachable!(),
				};

				match source
				{
					RegisterIndex::R0 => pre.r0 = equation.rhs,
					RegisterIndex::R1 => pre.r1 = equation.rhs,
					RegisterIndex::Lr => pre.lr = equation.rhs,
					RegisterIndex::Pc => pre.pc = equation.rhs,
				};

				let post: Registers = execute(pre.clone());

				assert_register(&equation_str, "ir", pre.ir, post.ir);

				match source
				{
					RegisterIndex::R0 =>
					{
						assert_register(&equation_str, "r0", equation.rhs, post.r0)
					}
					RegisterIndex::R1 =>
					{
						assert_register(&equation_str, "r1", equation.rhs, post.r1)
					}
					RegisterIndex::Lr =>
					{
						assert_register(&equation_str, "lr", equation.rhs, post.lr)
					}
					RegisterIndex::Pc =>
					{
						assert_register(&equation_str, "pc", equation.rhs, post.pc)
					}
				};

				match dest
				{
					RegisterIndex::R0 =>
					{
						assert_register(&equation_str, "r0", equation.result, post.r0)
					}
					RegisterIndex::R1 =>
					{
						assert_register(&equation_str, "r1", equation.result, post.r1)
					}
					RegisterIndex::Lr =>
					{
						assert_register(&equation_str, "lr", equation.result, post.lr)
					}
					RegisterIndex::Pc => unreachable!(),
				};

				let expected_sr: Word = 0x0000;
				let expected_sr: Word =
					StatusRegisterFlag::C.set_if(expected_sr, equation.overflow);
				let expected_sr: Word =
					StatusRegisterFlag::Z.set_if(expected_sr, equation.result == 0x0000);

				assert_register(&equation_str, "fr", 0x0000, post.fr);
				assert_register(&equation_str, "sp", 0x0000, post.sp);
				assert_register(&equation_str, "s0", 0x0000, post.s0);
				assert_register(&equation_str, "s1", 0x0000, post.s1);
				assert_register(&equation_str, "s2", 0x0000, post.s2);
				assert_register(&equation_str, "sr", expected_sr, post.sr);
			}
		}
	}
}

fn check_equations_for_pc_register(
	equations: &[Equation],
	operation: fn(RegisterIndex, RegisterIndex) -> Word,
)
{
	for equation in equations
	{
		for source in [RegisterIndex::R0, RegisterIndex::R1, RegisterIndex::Lr]
		{
			let equation_str: String = equation.description(source, RegisterIndex::Pc);
			let mut pre: Registers = Registers::default();

			pre.ir = operation(source, RegisterIndex::Pc);
			pre.pc = equation.lhs;

			match source
			{
				RegisterIndex::R0 => pre.r0 = equation.rhs,
				RegisterIndex::R1 => pre.r1 = equation.rhs,
				RegisterIndex::Lr => pre.lr = equation.rhs,
				RegisterIndex::Pc => unreachable!(),
			};

			let post: Registers = execute(pre.clone());

			let expected_sr: Word = 0x0000;
			let expected_sr: Word = StatusRegisterFlag::C.set_if(expected_sr, equation.overflow);
			let expected_sr: Word =
				StatusRegisterFlag::Z.set_if(expected_sr, equation.result == 0x0000);

			assert_register(&equation_str, "ir", pre.ir, post.ir);

			match source
			{
				RegisterIndex::R0 => assert_register(&equation_str, "r0", equation.rhs, post.r0),
				RegisterIndex::R1 => assert_register(&equation_str, "r1", equation.rhs, post.r1),
				RegisterIndex::Lr => assert_register(&equation_str, "lr", equation.rhs, post.lr),
				RegisterIndex::Pc => unreachable!(),
			};

			assert_register(&equation_str, "pc", equation.result, post.pc);
			assert_register(&equation_str, "fr", 0x0000, post.fr);
			assert_register(&equation_str, "sp", 0x0000, post.sp);
			assert_register(&equation_str, "s0", 0x0000, post.s0);
			assert_register(&equation_str, "s1", 0x0000, post.s1);
			assert_register(&equation_str, "s2", 0x0000, post.s2);
			assert_register(&equation_str, "sr", expected_sr, post.sr);
		}
	}
}

fn check_literal_op_for_non_pc_registers<Op>(equations: &[Equation], operation: Op)
where
	Op: Fn(RegisterIndex, Byte) -> Word,
{
	for equation in equations
	{
		for dest in [RegisterIndex::R0, RegisterIndex::R1, RegisterIndex::Lr]
		{
			let equation_str: String = equation.description(dest, dest);
			let mut pre: Registers = Registers::default();

			pre.ir = operation(dest, equation.rhs as Byte);

			match dest
			{
				RegisterIndex::R0 => pre.r0 = equation.lhs,
				RegisterIndex::R1 => pre.r1 = equation.lhs,
				RegisterIndex::Lr => pre.lr = equation.lhs,
				RegisterIndex::Pc => unreachable!(),
			};

			let post: Registers = execute(pre.clone());

			assert_register(&equation_str, "ir", pre.ir, post.ir);

			assert_register(
				&equation_str,
				"r0",
				if dest == RegisterIndex::R0
				{
					equation.result
				}
				else
				{
					pre.r0
				},
				post.r0,
			);

			assert_register(
				&equation_str,
				"r1",
				if dest == RegisterIndex::R1
				{
					equation.result
				}
				else
				{
					pre.r1
				},
				post.r1,
			);

			assert_register(
				&equation_str,
				"lr",
				if dest == RegisterIndex::Lr
				{
					equation.result
				}
				else
				{
					pre.lr
				},
				post.lr,
			);

			assert_register(&equation_str, "pc", pre.pc, post.pc);

			let expected_sr: Word = 0x0000;
			let expected_sr: Word = StatusRegisterFlag::C.set_if(expected_sr, equation.overflow);
			let expected_sr: Word =
				StatusRegisterFlag::Z.set_if(expected_sr, equation.result == 0x0000);

			assert_register(&equation_str, "fr", 0x0000, post.fr);
			assert_register(&equation_str, "sp", 0x0000, post.sp);
			assert_register(&equation_str, "s0", 0x0000, post.s0);
			assert_register(&equation_str, "s1", 0x0000, post.s1);
			assert_register(&equation_str, "s2", 0x0000, post.s2);
			assert_register(&equation_str, "sr", expected_sr, post.sr);
		}
	}
}
