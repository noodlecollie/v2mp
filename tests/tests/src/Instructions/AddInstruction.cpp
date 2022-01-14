#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word VAL_R0 = 0x0101;
static constexpr V2MP_Word VAL_R1 = 0xBEEF;
static constexpr V2MP_Word VAL_LR = 0xB00B;
static constexpr V2MP_Word VAL_PC = 0x1234;

SCENARIO("ADD: Adding one register to another results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;
			static constexpr V2MP_Word VAL_DEST = VAL_R0;

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;
				static constexpr V2MP_Word VAL_SRC = VAL_R1;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("R0 is incremented by the value in R1, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;
				static constexpr V2MP_Word VAL_SRC = VAL_LR;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("R0 is incremented by the value in LR, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;
				static constexpr V2MP_Word VAL_SRC = VAL_PC;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("R0 is incremented by the value in PC, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
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
			static constexpr uint8_t REG_DEST = Asm::REG_R1;
			static constexpr V2MP_Word VAL_DEST = VAL_R1;

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;
				static constexpr V2MP_Word VAL_SRC = VAL_R0;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("R1 is incremented by the value in R0, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;
				static constexpr V2MP_Word VAL_SRC = VAL_LR;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("R1 is incremented by the value in LR, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;
				static constexpr V2MP_Word VAL_SRC = VAL_PC;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("R1 is incremented by the value in PC, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;
			static constexpr V2MP_Word VAL_DEST = VAL_LR;

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;
				static constexpr V2MP_Word VAL_SRC = VAL_R0;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("LR is incremented by the value in R0, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;
				static constexpr V2MP_Word VAL_SRC = VAL_R1;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("LR is incremented by the value in R1, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;
				static constexpr V2MP_Word VAL_SRC = VAL_PC;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("LR is incremented by the value in PC, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK(vm.GetPC() == VAL_PC);
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;
			static constexpr V2MP_Word VAL_DEST = VAL_PC;

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;
				static constexpr V2MP_Word VAL_SRC = VAL_R0;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("PC is incremented by the number of words specified by R0, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_DEST + (sizeof(V2MP_Word) * VAL_SRC)));
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;
				static constexpr V2MP_Word VAL_SRC = VAL_R1;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("PC is incremented by the number of words specified by R1, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_DEST + (sizeof(V2MP_Word) * VAL_SRC)));
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;
				static constexpr V2MP_Word VAL_SRC = VAL_LR;

				WHEN("The add instruction is executed")
				{
					REQUIRE(vm.Execute(Asm::ADDR(REG_SRC, REG_DEST)));

					THEN("PC is incremented by the number of words specified by LR, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_DEST + (sizeof(V2MP_Word) * VAL_SRC)));
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}
	}
}

SCENARIO("ADD: Adding a literal to a register results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		static constexpr uint8_t INCREMENT = 0x43;

		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;
			static constexpr V2MP_Word VAL_DEST = VAL_R0;

			WHEN("A literal value is added")
			{
				REQUIRE(vm.Execute(Asm::ADDL(REG_DEST, INCREMENT)));

				THEN("R0 is incremented by the literal value, and other registers are unchanged")
				{
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(VAL_DEST + static_cast<V2MP_Word>(INCREMENT)));
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;
			static constexpr V2MP_Word VAL_DEST = VAL_R1;

			WHEN("A literal value is added")
			{
				REQUIRE(vm.Execute(Asm::ADDL(REG_DEST, INCREMENT)));

				THEN("R1 is incremented by the literal value, and other registers are unchanged")
				{
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(VAL_DEST + static_cast<V2MP_Word>(INCREMENT)));
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;
			static constexpr V2MP_Word VAL_DEST = VAL_LR;

			WHEN("A literal value is added")
			{
				REQUIRE(vm.Execute(Asm::ADDL(REG_DEST, INCREMENT)));

				THEN("LR is incremented by the literal value, and other registers are unchanged")
				{
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(VAL_DEST + static_cast<V2MP_Word>(INCREMENT)));
					CHECK(vm.GetPC() == VAL_PC);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;
			static constexpr V2MP_Word VAL_DEST = VAL_PC;

			WHEN("A literal value is added")
			{
				REQUIRE(vm.Execute(Asm::ADDL(REG_DEST, INCREMENT)));

				THEN("PC is incremented by this number of words, and other registers are unchanged")
				{
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_DEST + (sizeof(V2MP_Word) * static_cast<V2MP_Word>(INCREMENT))));
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("ADD: Adding one register value to another results in the correct status register bits being set", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		TestHarnessVM vm;

		WHEN("0x0 is added to 0x0")
		{
			vm.SetR0(0x0);
			vm.SetR1(0x0);

			REQUIRE(vm.Execute(Asm::ADDR(Asm::REG_R0, Asm::REG_R1)));
			REQUIRE(vm.GetR1() == static_cast<V2MP_Word>(0x0 + 0x0));

			THEN("SR[Z] is set, SR[C] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1 is added to 0x0")
		{
			vm.SetR0(0x1);
			vm.SetR1(0x0);

			REQUIRE(vm.Execute(Asm::ADDR(Asm::REG_R0, Asm::REG_R1)));
			REQUIRE(vm.GetR1() == static_cast<V2MP_Word>(0x0 + 0x1));

			THEN("SR[Z] is not set, SR[C] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x12 is added to 0x34")
		{
			vm.SetR0(0x12);
			vm.SetR1(0x34);

			REQUIRE(vm.Execute(Asm::ADDR(Asm::REG_R0, Asm::REG_R1)));
			REQUIRE(vm.GetR1() == static_cast<V2MP_Word>(0x12 + 0x34));

			THEN("SR[Z] is not set, SR[C] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1 is added to 0xFFFF")
		{
			vm.SetR0(0x1);
			vm.SetR1(0xFFFF);

			REQUIRE(vm.Execute(Asm::ADDR(Asm::REG_R0, Asm::REG_R1)));
			REQUIRE(vm.GetR1() == static_cast<V2MP_Word>(0xFFFF + 0x1));

			THEN("SR[Z] is set, SR[C] is set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & Asm::SR_C) != 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x123 is added to 0xFFF0")
		{
			vm.SetR0(0x123);
			vm.SetR1(0xFFF0);

			REQUIRE(vm.Execute(Asm::ADDR(Asm::REG_R0, Asm::REG_R1)));
			REQUIRE(vm.GetR1() == static_cast<V2MP_Word>(0xFFF0 + 0x123));

			THEN("SR[Z] is not set, SR[C] is set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) != 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}
	}
}

SCENARIO("ADD: Adding a literal to a register results in the correct status register bits being set", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		TestHarnessVM vm;

		WHEN("0x0 is added to 0x0")
		{
			vm.SetR0(0x0);

			REQUIRE(vm.Execute(Asm::ADDL(Asm::REG_R0, 0x0)));
			REQUIRE(vm.GetR0() == static_cast<V2MP_Word>(0x0 + 0x0));

			THEN("SR[Z] is set, SR[C] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1 is added to 0x0")
		{
			vm.SetR0(0x0);

			REQUIRE(vm.Execute(Asm::ADDL(Asm::REG_R0, 0x1)));
			REQUIRE(vm.GetR0() == static_cast<V2MP_Word>(0x0 + 0x1));

			THEN("SR[Z] is not set, SR[C] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x12 is added to 0x34")
		{
			vm.SetR0(0x34);

			REQUIRE(vm.Execute(Asm::ADDL(Asm::REG_R0, 0x12)));
			REQUIRE(vm.GetR0() == static_cast<V2MP_Word>(0x12 + 0x34));

			THEN("SR[Z] is not set, SR[C] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) == 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1 is added to 0xFFFF")
		{
			vm.SetR0(0xFFFF);

			REQUIRE(vm.Execute(Asm::ADDL(Asm::REG_R0, 0x1)));
			REQUIRE(vm.GetR0() == static_cast<V2MP_Word>(0xFFFF + 0x1));

			THEN("SR[Z] is set, SR[C] is set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & Asm::SR_C) != 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0xFF is added to 0xFFF0")
		{
			vm.SetR0(0xFFF0);

			REQUIRE(vm.Execute(Asm::ADDL(Asm::REG_R0, 0xFF)));
			REQUIRE(vm.GetR0() == static_cast<V2MP_Word>(0xFFF0 + 0xFF));

			THEN("SR[Z] is not set, SR[C] is set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & Asm::SR_C) != 0);
				CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}
	}
}

SCENARIO("ADD: Setting any literal operand bit if the source and destination registers are different raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		for ( size_t index = 0; index <= 8; ++index )
		{
			WHEN("R1 is added to R0 with a reserved bit set")
			{
				REQUIRE(vm.Execute(Asm::ADDR(Asm::REG_R1, Asm::REG_R0) | (1 << index)));

				THEN("A RES fault is raised, and all registers are left unchanged")
				{
					INFO("Reserved bit " << index << " was set");
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
}
