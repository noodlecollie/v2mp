use crate::arch::*;
use crate::execution_context::Registers;
use crate::instructions::execute;
use crate::instructions::tests::asm;

fn assert_register(equation: &str, name: &str, expected: Word, actual: Word)
{
	assert_eq!(
		expected, actual,
		"Equation \"{equation}\": Expected register {name} to be {:#06x}, but it was {:#06x}",
		expected, actual
	);
}

#[test]
fn happy_add_to_non_pc_register()
{
	// a + b = c (true if overflow)
	const EQUATIONS: [(Word, Word, Word, bool); 8] = [
		// Simple small values
		(0x0001, 0x0001, 0x0002, false),
		(0x1000, 0x0001, 0x1001, false),
		// Overflow
		(0xFFFF, 0x0001, 0x0000, true),
		(0x0001, 0xFFFF, 0x0000, true),
		(0xFFFF, 0xFFFF, 0xFFFE, true),
		// Zero
		(0x1234, 0x0000, 0x1234, false),
		(0x0000, 0x5678, 0x5678, false),
		(0x0000, 0x0000, 0x0000, false),
	];

	for equation in EQUATIONS
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

				let equation_str: String = format!(
					"{:#06x} ({}) + {:#06x} ({}) = {:#06x} ({})",
					equation.0,
					dest.name(),
					equation.1,
					source.name(),
					equation.2,
					if equation.3
					{
						"overflow"
					}
					else
					{
						"no overflow"
					}
				);

				let mut pre: Registers = Registers::default();

				pre.ir = asm::addr(source, dest);

				match dest
				{
					RegisterIndex::R0 => pre.r0 = equation.0,
					RegisterIndex::R1 => pre.r1 = equation.0,
					RegisterIndex::Lr => pre.lr = equation.0,
					RegisterIndex::Pc => unreachable!(),
				};

				match source
				{
					RegisterIndex::R0 => pre.r0 = equation.1,
					RegisterIndex::R1 => pre.r1 = equation.1,
					RegisterIndex::Lr => pre.lr = equation.1,
					RegisterIndex::Pc => pre.pc = equation.1,
				};

				let post: Registers = execute(pre.clone());

				assert_register(&equation_str, "ir", pre.ir, post.ir);

				match source
				{
					RegisterIndex::R0 => assert_register(&equation_str, "r0", equation.1, post.r0),
					RegisterIndex::R1 => assert_register(&equation_str, "r1", equation.1, post.r1),
					RegisterIndex::Lr => assert_register(&equation_str, "lr", equation.1, post.lr),
					RegisterIndex::Pc => assert_register(&equation_str, "pc", equation.1, post.pc),
				};

				match dest
				{
					RegisterIndex::R0 => assert_register(&equation_str, "r0", equation.2, post.r0),
					RegisterIndex::R1 => assert_register(&equation_str, "r1", equation.2, post.r1),
					RegisterIndex::Lr => assert_register(&equation_str, "lr", equation.2, post.lr),
					RegisterIndex::Pc => unreachable!(),
				};

				let expected_sr: Word = 0x0000;
				let expected_sr: Word = StatusRegisterFlag::C.set_if(expected_sr, equation.3);
				let expected_sr: Word =
					StatusRegisterFlag::Z.set_if(expected_sr, equation.2 == 0x0000);

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

#[test]
fn happy_add_to_pc_register()
{
	// a + b = c (true if overflow)
	const EQUATIONS: [(Word, Word, Word, bool); 8] = [
		// Simple small values
		(0x0010, 0x0001, 0x0012, false),
		(0x1000, 0x0001, 0x1002, false),
		// Overflow
		(0xFFFE, 0x0001, 0x0000, true),
		(0x0000, 0xFFFF, 0xFFFE, true),
		(0xFFFE, 0xFFFF, 0xFFFC, true),
		// Zero
		(0x1234, 0x0000, 0x1234, false),
		(0x0000, 0x0020, 0x0040, false),
		(0x0000, 0x0000, 0x0000, false),
	];

	for equation in EQUATIONS
	{
		for source in [RegisterIndex::R0, RegisterIndex::R1, RegisterIndex::Lr]
		{
			let equation_str: String = format!(
				"{:#06x} ({}) + {:#06x} ({}) = {:#06x} ({})",
				equation.0,
				RegisterIndex::Pc.name(),
				equation.1,
				source.name(),
				equation.2,
				if equation.3
				{
					"overflow"
				}
				else
				{
					"no overflow"
				}
			);

			let mut pre: Registers = Registers::default();

			pre.ir = asm::addr(source, RegisterIndex::Pc);
			pre.pc = equation.0;

			match source
			{
				RegisterIndex::R0 => pre.r0 = equation.1,
				RegisterIndex::R1 => pre.r1 = equation.1,
				RegisterIndex::Lr => pre.lr = equation.1,
				RegisterIndex::Pc => unreachable!(),
			};

			let post: Registers = execute(pre.clone());

			let expected_sr: Word = 0x0000;
			let expected_sr: Word = StatusRegisterFlag::C.set_if(expected_sr, equation.3);
			let expected_sr: Word = StatusRegisterFlag::Z.set_if(expected_sr, equation.2 == 0x0000);

			assert_register(&equation_str, "ir", pre.ir, post.ir);

			match source
			{
				RegisterIndex::R0 => assert_register(&equation_str, "r0", equation.1, post.r0),
				RegisterIndex::R1 => assert_register(&equation_str, "r1", equation.1, post.r1),
				RegisterIndex::Lr => assert_register(&equation_str, "lr", equation.1, post.lr),
				RegisterIndex::Pc => unreachable!(),
			};

			assert_register(&equation_str, "pc", equation.2, post.pc);
			assert_register(&equation_str, "fr", 0x0000, post.fr);
			assert_register(&equation_str, "sp", 0x0000, post.sp);
			assert_register(&equation_str, "s0", 0x0000, post.s0);
			assert_register(&equation_str, "s1", 0x0000, post.s1);
			assert_register(&equation_str, "s2", 0x0000, post.s2);
			assert_register(&equation_str, "sr", expected_sr, post.sr);
		}
	}
}
