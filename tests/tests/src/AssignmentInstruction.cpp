#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word VAL_R0 = 0x0001;
static constexpr V2MP_Word VAL_R1 = 0x0002;
static constexpr V2MP_Word VAL_LR = 0x0003;
static constexpr V2MP_Word VAL_PC = 0x0004;

SCENARIO("Assigning values between different registers results in the correct value being assigned", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		MinimalVirtualMachine vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REGDEST = Asm::REG_R0;

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REGSRC = Asm::REG_R1;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("R0 holds the value that was in R1, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R1);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REGSRC = Asm::REG_LR;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("R0 holds the value that was in LR, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_LR);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REGSRC = Asm::REG_PC;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("R0 holds the value that was in PC, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_PC);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REGDEST = Asm::REG_R1;

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REGSRC = Asm::REG_R0;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("R1 holds the value that was in R0, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R0);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REGSRC = Asm::REG_LR;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("R1 holds the value that was in LR, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_LR);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REGSRC = Asm::REG_PC;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("R1 holds the value that was in PC, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_PC);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REGDEST = Asm::REG_LR;

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REGSRC = Asm::REG_R0;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("LR holds the value that was in R0, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_R0);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REGSRC = Asm::REG_R1;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("LR holds the value that was in R1, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_R1);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REGSRC = Asm::REG_PC;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("LR holds the value that was in PC, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_PC);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REGDEST = Asm::REG_PC;

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REGSRC = Asm::REG_R0;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("PC holds the value that was in R0, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_R0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REGSRC = Asm::REG_R1;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("PC holds the value that was in R1, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_R1);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REGSRC = Asm::REG_LR;

				WHEN("The assignment instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ASGNR(REGSRC, REGDEST)));

					THEN("PC holds the value that was in LR, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_LR);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}
	}
}

SCENARIO("Assigning a literal value to a register results in the correct value being assigned", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		// Just to avoid casting between types, to make doubly sure
		// we're comparing the values we expect to be comparing:
		static constexpr int8_t POSITIVE_LITERAL = 42;
		static constexpr int16_t POSITIVE_VALUE = 42;

		static constexpr int8_t NEGATIVE_LITERAL = -33;
		static constexpr int16_t NEGATIVE_VALUE = -33;

		MinimalVirtualMachine vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REGDEST = Asm::REG_R0;

			WHEN("A positive literal is assigned")
			{
				REQUIRE(vm.Execute(Asm::ASGNL(REGDEST, POSITIVE_LITERAL)));

				THEN("R0 holds this value, and all other registers are unchanged")
				{
					CHECK(static_cast<int16_t>(vm.GetR0()) == POSITIVE_VALUE);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A negative literal is assigned")
			{
				REQUIRE(vm.Execute(Asm::ASGNL(REGDEST, NEGATIVE_LITERAL)));

				THEN("R0 holds this value, and all other registers are unchanged")
				{
					CHECK(static_cast<int16_t>(vm.GetR0()) == NEGATIVE_VALUE);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REGDEST = Asm::REG_R1;

			WHEN("A positive literal is assigned")
			{
				REQUIRE(vm.Execute(Asm::ASGNL(REGDEST, POSITIVE_LITERAL)));

				THEN("R1 holds this value, and all other registers are unchanged")
				{
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(static_cast<int16_t>(vm.GetR1()) == POSITIVE_VALUE);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A negative literal is assigned")
			{
				REQUIRE(vm.Execute(Asm::ASGNL(REGDEST, NEGATIVE_LITERAL)));

				THEN("R1 holds this value, and all other registers are unchanged")
				{
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(static_cast<int16_t>(vm.GetR1()) == NEGATIVE_VALUE);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REGDEST = Asm::REG_LR;

			WHEN("A positive literal is assigned")
			{
				REQUIRE(vm.Execute(Asm::ASGNL(REGDEST, POSITIVE_LITERAL)));

				THEN("LR holds this value, and all other registers are unchanged")
				{
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(static_cast<int16_t>(vm.GetLR()) == POSITIVE_VALUE);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A negative literal is assigned")
			{
				REQUIRE(vm.Execute(Asm::ASGNL(REGDEST, NEGATIVE_LITERAL)));

				THEN("LR holds this value, and all other registers are unchanged")
				{
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(static_cast<int16_t>(vm.GetLR()) == NEGATIVE_VALUE);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}
