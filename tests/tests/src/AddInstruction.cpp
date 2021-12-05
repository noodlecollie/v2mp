#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

SCENARIO("Adding one register to another results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
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

					THEN("PC is incremented by the value in R0, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
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

					THEN("PC is incremented by the value in R1, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
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

					THEN("PC is incremented by the value in LR, and other registers are unchanged")
					{
						CHECK(vm.GetR0() == VAL_R0);
						CHECK(vm.GetR1() == VAL_R1);
						CHECK(vm.GetLR() == VAL_LR);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_DEST + VAL_SRC));
						CHECK_FALSE(vm.CPUHasFault());
					}
				}
			}
		}
	}
}

SCENARIO("Adding a literal to a non-PC register results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0101;
		static constexpr V2MP_Word VAL_R1 = 0xBEEF;
		static constexpr V2MP_Word VAL_LR = 0xB00B;
		static constexpr V2MP_Word VAL_PC = 0x1234;
		static constexpr uint8_t INCREMENT = 0x43;

		MinimalVirtualMachine vm;

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
	}
}

SCENARIO("Adding a literal to PC results in the correct value in the register", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0101;
		static constexpr V2MP_Word VAL_R1 = 0xBEEF;
		static constexpr V2MP_Word VAL_LR = 0xB00B;
		static constexpr V2MP_Word VAL_PC = 0x1234;
		static constexpr uint8_t INCREMENT = 0x43;

		MinimalVirtualMachine vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("A literal value is added")
		{
			REQUIRE(vm.Execute(Asm::ADDL(Asm::REG_PC, INCREMENT)));

			THEN("PC is incremented by this number of words, and other registers are unchanged")
			{
				CHECK(vm.GetR0() == VAL_R0);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == static_cast<V2MP_Word>(VAL_PC + (sizeof(V2MP_Word) * static_cast<V2MP_Word>(INCREMENT))));
				CHECK_FALSE(vm.CPUHasFault());
			}
		}
	}
}

// TODO: Tests for SR bits
// TODO: Tests for reserved bits
