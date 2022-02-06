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

SCENARIO("MUL: Performing a multiplication sets the status register appropriately", "[instructions]")
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

			WHEN("A non-zero unsigned multiplication is performed that fits entirely into the lower register")
			{
				vm.SetR0(2);
				vm.SetR1(2);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("SR[Z] is not set and SR[C] is not set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A non-zero unsigned multiplication is performed that does not fit into the lower register")
			{
				vm.SetR0(0xFFFF);
				vm.SetR1(2);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("SR[Z] is not set and SR[C] is set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("An unsigned multiplication is performed that results in a value of zero")
			{
				vm.SetR0(0xFFFF);
				vm.SetR1(0);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("SR[Z] is set and SR[C] is not set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A positive signed multiplication is performed that fits entirely into the lower register")
			{
				vm.SetR0(2);
				vm.SetR1(2);

				REQUIRE(vm.Execute(Asm::IMULR(REG_DEST)));

				THEN("SR[Z] is not set and SR[C] is not set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A positive signed multiplication is performed that does not fit into the lower register")
			{
				vm.SetR0(32767);
				vm.SetR1(32767);

				REQUIRE(vm.Execute(Asm::IMULR(REG_DEST)));

				THEN("SR[Z] is not set and SR[C] is set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A negative signed multiplication is performed that fits entirely into the lower register")
			{
				vm.SetR0(-2);
				vm.SetR1(2);

				REQUIRE(vm.Execute(Asm::IMULR(REG_DEST)));

				THEN("SR[Z] is not set and SR[C] is not set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A negative signed multiplication is performed that does not fit into the lower register")
			{
				vm.SetR0(-32767);
				vm.SetR1(32767);

				REQUIRE(vm.Execute(Asm::IMULR(REG_DEST)));

				THEN("SR[Z] is not set and SR[C] is set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A signed multiplication is performed that results in a value of zero")
			{
				vm.SetR0(-32768);
				vm.SetR1(0);

				REQUIRE(vm.Execute(Asm::MULR(REG_DEST)));

				THEN("SR[Z] is set and SR[C] is not set")
				{
					CHECK_FALSE(vm.CPUHasFault());
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}
	}
}

SCENARIO("MUL: Setting any reserved bit raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0001;
		static constexpr V2MP_Word VAL_R1 = 0x0002;
		static constexpr V2MP_Word VAL_LR = 0x0003;
		static constexpr V2MP_Word VAL_PC = 0x0004;

		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("A DPO instruction is executed with a reserved bit set")
		{
			REQUIRE(vm.Execute(Asm::MULR(Asm::REG_R0) | (1 << 8)));

			THEN("A RES fault is raised, and all registers are left unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
				CHECK(vm.GetR0() == VAL_R0);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
				CHECK(vm.GetSR() == 0);
			}
		}
	}
}
