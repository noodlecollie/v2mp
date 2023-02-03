#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "TestUtil/Assembly.h"

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

				vm.SetR0(static_cast<V2MP_Word>(LHS));
				vm.SetR1(static_cast<V2MP_Word>(RHS));

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

				vm.SetR0(static_cast<V2MP_Word>(LHS));
				vm.SetR1(static_cast<V2MP_Word>(RHS));

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

				vm.SetR0(static_cast<V2MP_Word>(LHS));
				vm.SetR1(static_cast<V2MP_Word>(RHS));

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

				vm.SetR0(static_cast<V2MP_Word>(LHS));

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

				vm.SetR0(static_cast<V2MP_Word>(LHS));

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

				vm.SetR0(static_cast<V2MP_Word>(LHS));

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

				vm.SetR0(static_cast<V2MP_Word>(LHS));

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

SCENARIO("DIV: Dividing by zero raises a DIV fault", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		static constexpr V2MP_Word NUMERATOR = 1234;

		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("A division instruction is executed where the source register is zero")
		{
			vm.SetR0(NUMERATOR);
			vm.SetR1(0);

			REQUIRE(vm.Execute(Asm::DIVR(Asm::REG_R0)));

			THEN("A DIV fault is raised, and all registers are left unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(vm.GetR0() == NUMERATOR);
				CHECK(vm.GetR1() == 0);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
			}
		}

		AND_WHEN("A division instruction is executed where the instruction literal is zero")
		{
			vm.SetR0(NUMERATOR);

			REQUIRE(vm.Execute(Asm::DIVL(Asm::REG_R0, 0)));

			THEN("A DIV fault is raised, and all registers are left unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(vm.GetR0() == NUMERATOR);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
			}
		}
	}
}

SCENARIO("DIV: Performing a division sets the status register appropriately", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("A division is performed where both the quotient and remainder are zero")
		{
			vm.SetR0(0);

			REQUIRE(vm.Execute(Asm::DIVL(Asm::REG_R0, 2)));

			THEN("SR[Z] is set and SR[C] is not set")
			{
				CHECK_FALSE(vm.CPUHasFault());
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
			}
		}

		AND_WHEN("A division is performed where the quotient is zero and the remainder is not zero")
		{
			vm.SetR0(2);

			REQUIRE(vm.Execute(Asm::DIVL(Asm::REG_R0, 3)));

			THEN("SR[Z] is set and SR[C] is set")
			{
				CHECK_FALSE(vm.CPUHasFault());
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & Asm::SR_C) != 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
			}
		}

		AND_WHEN("A division is performed where the quotient is not zero and the remainder is zero")
		{
			vm.SetR0(4);

			REQUIRE(vm.Execute(Asm::DIVL(Asm::REG_R0, 2)));

			THEN("SR[Z] is not set and SR[C] is not set")
			{
				CHECK_FALSE(vm.CPUHasFault());
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
			}
		}

		AND_WHEN("A division is performed where the quotient is not zero and the remainder is not zero")
		{
			vm.SetR0(5);

			REQUIRE(vm.Execute(Asm::DIVL(Asm::REG_R0, 2)));

			THEN("SR[Z] is not set and SR[C] is set")
			{
				CHECK_FALSE(vm.CPUHasFault());
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) != 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
			}
		}
	}
}

SCENARIO("DIV: Setting any reserved bit raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("A DIV instruction is executed with a reserved bit set")
		{
			REQUIRE(vm.Execute(Asm::DIVR(Asm::REG_R0) | (1 << 8)));

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
