#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word BIT_0 = 0x0001;
static constexpr V2MP_Word BIT_4 = 0x0010;
static constexpr V2MP_Word BIT_6 = 0x0040;
static constexpr V2MP_Word BIT_9 = 0x0200;
static constexpr V2MP_Word BIT_11 = 0x0800;
static constexpr V2MP_Word BIT_15 = 0x8000;
static constexpr V2MP_Word SHIFT_LEFT_FOUR = 0x0004;
static constexpr V2MP_Word SHIFT_RIGHT_FOUR = 0xFFFC;

SCENARIO("SHFT: Shifting register bits left by the value in another register results in the correct value", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(BIT_0);

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				vm.SetR1(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == BIT_4);
						CHECK(vm.GetR1() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				vm.SetLR(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == BIT_4);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				vm.SetPC(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == BIT_4);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == SHIFT_LEFT_FOUR);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			vm.SetR1(BIT_0);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				vm.SetR0(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetR1() == BIT_4);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				vm.SetLR(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == BIT_4);
						CHECK(vm.GetLR() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				vm.SetPC(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == BIT_4);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == SHIFT_LEFT_FOUR);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			vm.SetLR(BIT_0);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				vm.SetR0(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == BIT_4);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				vm.SetR1(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetLR() == BIT_4);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				vm.SetPC(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == BIT_4);
						CHECK(vm.GetPC() == SHIFT_LEFT_FOUR);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			vm.SetPC(BIT_0);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				vm.SetR0(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == BIT_4);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				vm.SetR1(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == BIT_4);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				vm.SetLR(SHIFT_LEFT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == SHIFT_LEFT_FOUR);
						CHECK(vm.GetPC() == BIT_4);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}
	}
}

SCENARIO("SHFT: Shifting register bits right by the value in another register results in the correct value", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(BIT_15);

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				vm.SetR1(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == BIT_11);
						CHECK(vm.GetR1() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				vm.SetLR(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == BIT_11);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				vm.SetPC(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == BIT_11);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == SHIFT_RIGHT_FOUR);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			vm.SetR1(BIT_15);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				vm.SetR0(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetR1() == BIT_11);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				vm.SetLR(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == BIT_11);
						CHECK(vm.GetLR() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				vm.SetPC(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == BIT_11);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == SHIFT_RIGHT_FOUR);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			vm.SetLR(BIT_15);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				vm.SetR0(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == BIT_11);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				vm.SetR1(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetLR() == BIT_11);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				vm.SetPC(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == BIT_11);
						CHECK(vm.GetPC() == SHIFT_RIGHT_FOUR);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			vm.SetPC(BIT_15);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				vm.SetR0(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == BIT_11);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				vm.SetR1(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == BIT_11);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				vm.SetLR(SHIFT_RIGHT_FOUR);

				WHEN("The instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::SHFTR(REG_SRC, REG_DEST)));

					THEN("The destination register value is correct, and all other registers are left unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == SHIFT_RIGHT_FOUR);
						CHECK(vm.GetPC() == BIT_11);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}
	}
}

SCENARIO("SHFT: Shifting register bits left by a literal value results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(BIT_0);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_4);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_9);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_15);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			vm.SetR1(BIT_0);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_4);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_9);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_15);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			vm.SetLR(BIT_0);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_4);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_9);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_15);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			vm.SetPC(BIT_0);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_4);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_9);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of 15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_15);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("SHFT: Shifting register bits right by a literal value results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(BIT_15);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_15);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_11);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_6);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == BIT_0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -16 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -16)));

				THEN("The destination register value is zero, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			vm.SetR1(BIT_15);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_15);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_11);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_6);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == BIT_0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -16 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -16)));

				THEN("The destination register value is zero, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			vm.SetLR(BIT_15);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_15);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_11);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_6);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == BIT_0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -16 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -16)));

				THEN("The destination register value is zero, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			vm.SetPC(BIT_15);

			WHEN("A left shift of 0 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, 0)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_15);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -4 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -4)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_11);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -9 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -9)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_6);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -15 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -15)));

				THEN("The destination register value is correct, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BIT_0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A left shift of -16 is executed")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(REG_DEST, -16)));

				THEN("The destination register value is zero, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("SHFT: Performing a bit shift sets the status register appropriately", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		AND_GIVEN("An empty register")
		{
			vm.SetR0(0x0000);

			WHEN("The register is shifted left")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, 3)));

				THEN("SR[Z] is set and SR[C] is not set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted right")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, -3)));

				THEN("SR[Z] is set and SR[C] is not set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("A register with bit 7 set")
		{
			vm.SetR0(0x0080);

			WHEN("The register is shifted left by 4")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, 4)));

				THEN("SR[Z] is not set and SR[C] is not set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted right by 4")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, -4)));

				THEN("SR[Z] is not set and SR[C] is not set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted left by 12")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, 12)));

				THEN("SR[Z] is set and SR[C] is set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted right by 12")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, -12)));

				THEN("SR[Z] is set and SR[C] is set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("A register with bits 3 and 12 set")
		{
			vm.SetR0(0x1008);

			WHEN("The register is shifted left by 3")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, 3)));

				THEN("SR[Z] is not set and SR[C] is not set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted right by 3")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, -3)));

				THEN("SR[Z] is not set and SR[C] is not set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted left by 5")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, 5)));

				THEN("SR[Z] is not set and SR[C] is set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted right by 5")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, -5)));

				THEN("SR[Z] is not set and SR[C] is set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("A register with all bits set")
		{
			vm.SetR0(0xFFFF);

			WHEN("The register is shifted left by 1")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, 1)));

				THEN("SR[Z] is not set and SR[C] is set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("The register is shifted right by 1")
			{
				REQUIRE(vm.Execute(Asm::SHFTL(Asm::REG_R0, -1)));

				THEN("SR[Z] is not set and SR[C] is set")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("SHFT: Setting any literal operand bit if the source and destination registers are different raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0101;
		static constexpr V2MP_Word VAL_R1 = 0xBEEF;
		static constexpr V2MP_Word VAL_LR = 0xB00B;
		static constexpr V2MP_Word VAL_PC = 0x1234;

		MinimalVirtualMachine vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		for ( size_t index = 0; index <= 8; ++index )
		{
			WHEN("R1 is shifted by the value in R0, and a reserved bit set")
			{
				REQUIRE(vm.Execute(Asm::SHFTR(Asm::REG_R1, Asm::REG_R0) | (1 << index)));

				THEN("A RES fault is raised, and all registers are left unchanged")
				{
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFault()) == V2MP_FAULT_RES);
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK(vm.GetSR() == 0);
				}
			}
		}
	}
}
