#include <limits>
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

SCENARIO("MUL: High and low portions of result are placed into the correct registers", "[instructions]")
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

			WHEN("A multiplication is performed where the result fits into one register")
			{
				static constexpr V2MP_Word RHS = 2;
				static constexpr V2MP_Word LHS = (static_cast<V2MP_Word>(~0) - 1) / RHS;

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(~0) - 1);
					CHECK(vm.GetR1() == RHS);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A multiplication is performed where the result does not fit into one register")
			{
				static constexpr V2MP_Word RHS = 2;
				static constexpr V2MP_Word LHS = static_cast<V2MP_Word>(static_cast<uint32_t>(0x00010000) / RHS);

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == 0x0000);
					CHECK(vm.GetR1() == RHS);
					CHECK(vm.GetLR() == 0x0001);
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("The largest mutliplication possible is performed")
			{
				static constexpr V2MP_Word RHS = 0xFFFF;
				static constexpr V2MP_Word LHS = 0xFFFF;

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == 0x0001);
					CHECK(vm.GetR1() == RHS);
					CHECK(vm.GetLR() == 0xFFFE);
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A multiplcation by zero is performed")
			{
				static constexpr V2MP_Word RHS = 0xFFFF;
				static constexpr V2MP_Word LHS = 0x0;

				vm.SetR0(LHS);
				vm.SetR1(RHS);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == 0x0);
					CHECK(vm.GetR1() == RHS);
					CHECK(vm.GetLR() == 0x0);
					CHECK(vm.GetPC() == VAL_PC);
				}
			}
		}
	}
}

SCENARIO("MUL: Signed multiplications are computed correctly", "[instructions]")
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

			WHEN("A register multiplication is performed where the result fits into one register")
			{
				static constexpr int32_t LHS_INT = -5;
				static constexpr int32_t RHS_INT = 3;

				// Sanity: the instruction treats both values as signed. Make sure they are in range.
				REQUIRE(LHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(LHS_INT >= std::numeric_limits<int16_t>::min());
				REQUIRE(RHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(RHS_INT >= std::numeric_limits<int16_t>::min());

				vm.SetR0(static_cast<V2MP_Word>(LHS_INT));
				vm.SetR1(static_cast<V2MP_Word>(RHS_INT));

				REQUIRE(vm.Execute(Asm::IMULR(REG_DEST)));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(static_cast<int32_t>(LHS_INT * RHS_INT) & 0x0000FFFF));
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(RHS_INT));
					CHECK(vm.GetLR() == static_cast<V2MP_Word>((static_cast<int32_t>(LHS_INT * RHS_INT) & 0xFFFF0000) >> 16));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A register multiplication is performed where the result does not fit into one register")
			{
				static constexpr int32_t LHS_INT = -32768;
				static constexpr int32_t RHS_INT = 4;

				// Sanity: the instruction treats both values as signed. Make sure they are in range.
				REQUIRE(LHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(LHS_INT >= std::numeric_limits<int16_t>::min());
				REQUIRE(RHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(RHS_INT >= std::numeric_limits<int16_t>::min());

				vm.SetR0(static_cast<V2MP_Word>(LHS_INT));
				vm.SetR1(static_cast<V2MP_Word>(RHS_INT));

				REQUIRE(vm.Execute(Asm::IMULR(REG_DEST)));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(static_cast<int32_t>(LHS_INT * RHS_INT) & 0x0000FFFF));
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(RHS_INT));
					CHECK(vm.GetLR() == static_cast<V2MP_Word>((static_cast<int32_t>(LHS_INT * RHS_INT) & 0xFFFF0000) >> 16));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			WHEN("A literal multiplication is performed where the result fits into one register")
			{
				static constexpr int32_t LHS_INT = -5;
				static constexpr int32_t RHS_INT = 3;

				// Sanity: the instruction treats both values as signed. Make sure they are in range.
				REQUIRE(LHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(LHS_INT >= std::numeric_limits<int16_t>::min());
				REQUIRE(RHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(RHS_INT >= std::numeric_limits<int16_t>::min());

				vm.SetR0(static_cast<V2MP_Word>(LHS_INT));

				REQUIRE(vm.Execute(Asm::IMULL(REG_DEST, static_cast<int8_t>(RHS_INT))));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(static_cast<int32_t>(LHS_INT * RHS_INT) & 0x0000FFFF));
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>((static_cast<int32_t>(LHS_INT * RHS_INT) & 0xFFFF0000) >> 16));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}

			AND_WHEN("A register multiplication is performed where the result does not fit into one register")
			{
				static constexpr int32_t LHS_INT = 32760;
				static constexpr int32_t RHS_INT = -4;

				// Sanity: the instruction treats both values as signed. Make sure they are in range.
				REQUIRE(LHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(LHS_INT >= std::numeric_limits<int16_t>::min());
				REQUIRE(RHS_INT <= std::numeric_limits<int16_t>::max());
				REQUIRE(RHS_INT >= std::numeric_limits<int16_t>::min());

				vm.SetR0(static_cast<V2MP_Word>(LHS_INT));

				REQUIRE(vm.Execute(Asm::IMULL(REG_DEST, static_cast<int8_t>(RHS_INT))));

				THEN("The values in the lower and upper registers are correct")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(static_cast<int32_t>(LHS_INT * RHS_INT) & 0x0000FFFF));
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>((static_cast<int32_t>(LHS_INT * RHS_INT) & 0xFFFF0000) >> 16));
					CHECK(vm.GetPC() == VAL_PC);
				}
			}
		}
	}
}

// TODO: SR checks
// TODO: Reserved bit checks
