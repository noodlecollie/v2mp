#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word VAL_R0 = 0x0101;
static constexpr V2MP_Word VAL_R1 = 0xBEEF;
static constexpr V2MP_Word VAL_LR = 0xB00B;
static constexpr V2MP_Word VAL_PC = 0x1234;

SCENARIO("MUL: Multiplying a register by a value results in the correct value in the destination registers", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		static constexpr V2MP_Word LHS = 13;
		static constexpr V2MP_Word RHS = 3;
		static constexpr V2MP_Word RESULT = LHS * RHS;

		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(LHS);

			AND_GIVEN("The source is R1")
			{
				vm.SetR1(RHS);

				WHEN("The multiplication instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == RESULT);
						CHECK(vm.GetR1() == RHS);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == VAL_PC);
					}
				}
			}

			AND_GIVEN("The source is the instruction word")
			{
				WHEN("The multiplication instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::MULL(REG_DEST, static_cast<uint8_t>(RHS))));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == RESULT);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == VAL_PC);
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			vm.SetR1(LHS);

			AND_GIVEN("The source is R1")
			{
				vm.SetR0(RHS);

				WHEN("The multiplication instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == RHS);
						CHECK(vm.GetR1() == RESULT);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == VAL_PC);
					}
				}
			}

			AND_GIVEN("The source is the instruction word")
			{
				WHEN("The multiplication instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::MULL(REG_DEST, static_cast<uint8_t>(RHS))));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == RESULT);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == VAL_PC);
					}
				}
			}
		}
	}
}

// TODO: Edge cases regarding splitting result across registers
// TODO: SR checks
// TODO: Reserved bit checks
