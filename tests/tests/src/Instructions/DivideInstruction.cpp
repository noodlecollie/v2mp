#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word VAL_R0 = 0x0101;
static constexpr V2MP_Word VAL_R1 = 0xBEEF;
static constexpr V2MP_Word VAL_LR = 0xB00B;
static constexpr V2MP_Word VAL_PC = 0x1234;

SCENARIO("DIV: Dividing a register by a value results in the correct values in the destination registers", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		static constexpr V2MP_Word LHS = 13;
		static constexpr V2MP_Word RHS = 3;
		static constexpr V2MP_Word RESULT_Q = LHS / RHS;
		static constexpr V2MP_Word RESULT_R = LHS % RHS;

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

				WHEN("The division instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::DIVR(REG_DEST)));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == RESULT_Q);
						CHECK(vm.GetR1() == RHS);
						CHECK(vm.GetLR() == RESULT_R);
						CHECK(vm.GetPC() == VAL_PC);
					}
				}
			}

			AND_GIVEN("The source is the instruction word")
			{
				WHEN("The division instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::DIVL(REG_DEST, static_cast<uint8_t>(RHS))));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == RESULT_Q);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == RESULT_R);
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

				WHEN("The division instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::DIVR(REG_DEST)));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == RHS);
						CHECK(vm.GetR1() == RESULT_Q);
						CHECK(vm.GetLR() == RESULT_R);
						CHECK(vm.GetPC() == VAL_PC);
					}
				}
			}

			AND_GIVEN("The source is the instruction word")
			{
				WHEN("The division instrution is executed")
				{
					REQUIRE(vm.Execute(Asm::DIVL(REG_DEST, static_cast<uint8_t>(RHS))));

					THEN("The values placed into registers are correct")
					{
						CHECK_FALSE(vm.CPUHasFault());
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == RESULT_Q);
						CHECK(vm.GetLR() == RESULT_R);
						CHECK(vm.GetPC() == VAL_PC);
					}
				}
			}
		}
	}
}

SCENARIO("DIV: Signed divisions are computed correctly", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		AND_GIVEN("The destination register is R0 and the source is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			WHEN("A positive numerator is divided by a positive denominator")
			{
				static constexpr int16_t LHS = 13;
				static constexpr int16_t RHS = 3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::IDIVR(REG_DEST)));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(RHS));
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A positive numerator is divided by a negative denominator")
			{
				static constexpr int16_t LHS = 13;
				static constexpr int16_t RHS = -3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::IDIVR(REG_DEST)));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(RHS));
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A negative numerator is divided by a positive denominator")
			{
				static constexpr int16_t LHS = -13;
				static constexpr int16_t RHS = 3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::IDIVR(REG_DEST)));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(RHS));
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A negative numerator is divided by a negative denominator")
			{
				static constexpr int16_t LHS = -13;
				static constexpr int16_t RHS = -3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::IDIVR(REG_DEST)));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(RHS));
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}
		}

		AND_GIVEN("The destination register is R0 and the source is the instruction literal")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			WHEN("A positive numerator is divided by a positive denominator")
			{
				static constexpr int16_t LHS = 13;
				static constexpr int16_t RHS = 3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);

				REQUIRE(vm.Execute(Asm::IDIVL(REG_DEST, static_cast<int8_t>(RHS))));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A positive numerator is divided by a negative denominator")
			{
				static constexpr int16_t LHS = 13;
				static constexpr int16_t RHS = -3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);

				REQUIRE(vm.Execute(Asm::IDIVL(REG_DEST, static_cast<int8_t>(RHS))));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A negative numerator is divided by a positive denominator")
			{
				static constexpr int16_t LHS = -13;
				static constexpr int16_t RHS = 3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);

				REQUIRE(vm.Execute(Asm::IDIVL(REG_DEST, static_cast<int8_t>(RHS))));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A negative numerator is divided by a negative denominator")
			{
				static constexpr int16_t LHS = -13;
				static constexpr int16_t RHS = -3;
				static constexpr int16_t RESULT_Q = LHS / RHS;
				static constexpr int16_t RESULT_R = LHS % RHS;

				vm.SetR0(LHS);

				REQUIRE(vm.Execute(Asm::IDIVL(REG_DEST, static_cast<int8_t>(RHS))));

				THEN("The values placed into registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(RESULT_Q));
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(RESULT_R));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}
		}
	}
}

// TODO: Division by 0
// TODO: Status register
// TODO: Reserved bits
