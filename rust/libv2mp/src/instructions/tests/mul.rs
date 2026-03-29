use super::utils::{DoubleEquation, assert_register, execute_and_check_fault};
use crate::arch::*;
use crate::execution_context::Registers;
use crate::instructions::execute;
use crate::instructions::tests::asm;

#[test]
fn happy_mul_register_unsigned()
{
	// a * b = c (true if overflow)
	const EQUATIONS: [DoubleEquation; 11] = [
		// Simple small values
		DoubleEquation::new_mulr(0x0001, 0x0002, 0x00000002, false),
		DoubleEquation::new_mulr(0x0002, 0x0002, 0x00000004, false),
		DoubleEquation::new_mulr(0x0001, 0x0001, 0x00000001, false),
		DoubleEquation::new_mulr(100, 10, 1000, false),
		// Overflow
		DoubleEquation::new_mulr(0xFFF0, 0x0002, 0x0001FFE0, true),
		DoubleEquation::new_mulr(0xFFFF, 0x0010, 0x000FFFF0, true),
		DoubleEquation::new_mulr(0xFFFF, 0xFFFF, 0xFFFE0001, true),
		DoubleEquation::new_mulr(0x8000, 0x0002, 0x00010000, true),
		// Zero
		DoubleEquation::new_mulr(0xFFFF, 0x0000, 0x00000000, false),
		DoubleEquation::new_mulr(0x0000, 0x1234, 0x00000000, false),
		DoubleEquation::new_mulr(0x0000, 0x0000, 0x00000000, false),
	];

	check_equations_for_non_pc_registers(&EQUATIONS, CheckOperation::Register(asm::mulru));
}

#[test]
fn happy_mul_literal_unsigned()
{
	// a * b = c (true if overflow)
	const EQUATIONS: [DoubleEquation; 11] = [
		// Simple small values
		DoubleEquation::new_mull(0x0001, 0x02, 0x00000002, false),
		DoubleEquation::new_mull(0x0002, 0x02, 0x00000004, false),
		DoubleEquation::new_mull(0x0001, 0x01, 0x00000001, false),
		DoubleEquation::new_mull(100, 10, 1000, false),
		// Overflow
		DoubleEquation::new_mull(0xFFF0, 0x02, 0x0001FFE0, true),
		DoubleEquation::new_mull(0xFFFF, 0x10, 0x000FFFF0, true),
		DoubleEquation::new_mull(0xFFFF, 0xFF, 0x00FEFF01, true),
		DoubleEquation::new_mull(0x8000, 0x02, 0x00010000, true),
		// Zero
		DoubleEquation::new_mull(0xFFFF, 0x00, 0x00000000, false),
		DoubleEquation::new_mull(0x0000, 0x34, 0x00000000, false),
		DoubleEquation::new_mull(0x0000, 0x00, 0x00000000, false),
	];

	check_equations_for_non_pc_registers(&EQUATIONS, CheckOperation::Literal(asm::mullu));
}

#[test]
fn happy_mul_register_signed()
{
	// a * b = c (true if overflow)
	const EQUATIONS: [DoubleEquation; 24] = [
		// Simple small positive values
		DoubleEquation::new_mulr(0x0001, 0x0002, 0x00000002, false),
		DoubleEquation::new_mulr(0x0002, 0x0002, 0x00000004, false),
		DoubleEquation::new_mulr(0x0001, 0x0001, 0x00000001, false),
		DoubleEquation::new_mulr(100, 10, 1000, false),
		// Simple small negative values
		DoubleEquation::new_mulr(0xFFFE, 0x0002, 0xFFFFFFFC, false),
		DoubleEquation::new_mulr(0xFFFF, 0xFFFE, 0x00000002, false),
		DoubleEquation::new_mulr(0xFFFF, 0xFFFF, 0x00000001, false),
		DoubleEquation::new_mulr(0xFFFF, 0x0001, 0xFFFFFFFF, false),
		DoubleEquation::new_mulr(as_word(-100), 10, as_u32(-1000), false),
		DoubleEquation::new_mulr(as_word(-100), as_word(-10), 1000, false),
		// Overflow
		DoubleEquation::new_mulr(as_word(-16384), as_word(-4), 65536, true),
		DoubleEquation::new_mulr(as_word(-16384), 4, as_u32(-65536), true),
		DoubleEquation::new_mulr(16384, as_word(-4), as_u32(-65536), true),
		DoubleEquation::new_mulr(16384, 4, 65536, true),
		DoubleEquation::new_mulr(16384, 2, 32768, true),
		DoubleEquation::new_mulr(16384, as_word(-4), as_u32(-65536), true),
		DoubleEquation::new_mulr(16385, as_word(-2), as_u32(-32770), true),
		// Not quite overflow
		DoubleEquation::new_mulr(16383, 2, 32766, false),
		DoubleEquation::new_mulr(16384, as_word(-2), as_u32(-32768), false),
		// Zero
		DoubleEquation::new_mulr(0x7FFF, 0x0000, 0x00000000, false),
		DoubleEquation::new_mulr(0x0000, 0x7FFF, 0x00000000, false),
		DoubleEquation::new_mulr(0xFFFF, 0x0000, 0x00000000, false),
		DoubleEquation::new_mulr(0x0000, 0xFFFF, 0x00000000, false),
		DoubleEquation::new_mulr(0x0000, 0x0000, 0x00000000, false),
	];

	check_equations_for_non_pc_registers(&EQUATIONS, CheckOperation::Register(asm::mulrs));
}

#[test]
fn happy_mul_literal_signed()
{
	// a * b = c (true if overflow)
	const EQUATIONS: [DoubleEquation; 24] = [
		// Simple small positive values
		DoubleEquation::new_mull(0x0001, 0x02, 0x00000002, false),
		DoubleEquation::new_mull(0x0002, 0x02, 0x00000004, false),
		DoubleEquation::new_mull(0x0001, 0x01, 0x00000001, false),
		DoubleEquation::new_mull(100, 10, 1000, false),
		// Simple small negative values
		DoubleEquation::new_mull(0xFFFE, 0x02, 0xFFFFFFFC, false),
		DoubleEquation::new_mull(0xFFFF, 0xFE, 0x00000002, false),
		DoubleEquation::new_mull(0xFFFF, 0xFF, 0x00000001, false),
		DoubleEquation::new_mull(0xFFFF, 0x0001, 0xFFFFFFFF, false),
		DoubleEquation::new_mull(as_word(-100), 10, as_u32(-1000), false),
		DoubleEquation::new_mull(as_word(-100), as_byte(-10), 1000, false),
		// Overflow
		DoubleEquation::new_mull(as_word(-16384), as_byte(-4), 65536, true),
		DoubleEquation::new_mull(as_word(-16384), 4, as_u32(-65536), true),
		DoubleEquation::new_mull(16384, as_byte(-4), as_u32(-65536), true),
		DoubleEquation::new_mull(16384, 4, 65536, true),
		DoubleEquation::new_mull(16384, 2, 32768, true),
		DoubleEquation::new_mull(16384, as_byte(-4), as_u32(-65536), true),
		DoubleEquation::new_mull(16385, as_byte(-2), as_u32(-32770), true),
		// Not quite overflow
		DoubleEquation::new_mull(16383, 2, 32766, false),
		DoubleEquation::new_mull(16384, as_byte(-2), as_u32(-32768), false),
		// Zero
		DoubleEquation::new_mull(0x7FFF, 0x00, 0x00000000, false),
		DoubleEquation::new_mull(0x0000, 0x7F, 0x00000000, false),
		DoubleEquation::new_mull(0xFFFF, 0x00, 0x00000000, false),
		DoubleEquation::new_mull(0x0000, 0xFF, 0x00000000, false),
		DoubleEquation::new_mull(0x0000, 0x00, 0x00000000, false),
	];

	check_equations_for_non_pc_registers(&EQUATIONS, CheckOperation::Literal(asm::mulls));
}

#[test]
fn faults()
{
	// Reserved bits
	let mut registers: Registers = Registers::default();

	registers.ir = (OpCode::Mul.as_value() << 12) | (1 << 8);

	execute_and_check_fault(
		"Mul reserved bits",
		registers,
		FaultCode::Res.as_register_value(0),
	);

	// Literal when not in literal mode
	let mut registers: Registers = Registers::default();

	registers.ir = (OpCode::Mul.as_value() << 12) | 0x12;

	execute_and_check_fault(
		"Mul disallowed literal bits",
		registers,
		FaultCode::Res.as_register_value(0),
	);
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
			let expected_sr: Word = StatusRegisterFlag::C.set_if(expected_sr, equation.overflow);
			let expected_sr: Word =
				StatusRegisterFlag::Z.set_if(expected_sr, equation.result == 0x00000000);

			assert_register(&equation_str, "fr", 0x0000, post.fr);
			assert_register(&equation_str, "sp", 0x0000, post.sp);
			assert_register(&equation_str, "s0", 0x0000, post.s0);
			assert_register(&equation_str, "s1", 0x0000, post.s1);
			assert_register(&equation_str, "s2", 0x0000, post.s2);
			assert_register(&equation_str, "sr", expected_sr, post.sr);
		}
	}
}

const fn as_word(val: i16) -> Word
{
	return val as Word;
}

const fn as_byte(val: i8) -> Byte
{
	return val as Byte;
}

const fn as_u32(val: i32) -> u32
{
	return val as u32;
}
