#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word BIT_0 = 0x0001;
static constexpr V2MP_Word BIT_4 = 0x0010;
static constexpr V2MP_Word SHIFT_LEFT_FOUR = 0x0004;

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
