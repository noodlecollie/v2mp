#define OLD_TEST

#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word TEST_VALUE = 0xFED5;

// Note that source registers are (and should be) ignored for a NOT operation.
// This test scenario sets them in the instruction anyway, to ensure that they are ignored.
SCENARIO("BITW: Performing a bitwise NOT on a register results in the correct value in the register", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(TEST_VALUE);

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			vm.SetR1(TEST_VALUE);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			vm.SetLR(TEST_VALUE);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			vm.SetPC(TEST_VALUE);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				WHEN("A bitwise NOT is performed")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT)));

					THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(~TEST_VALUE));
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}
	}
}

SCENARIO("BITW: Performing a bitwise NOT between two registers with other operand bits set raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0001;
		static constexpr V2MP_Word VAL_R1 = 0x0002;
		static constexpr V2MP_Word VAL_LR = 0x0003;
		static constexpr V2MP_Word VAL_PC = 0x0004;

		MinimalVirtualMachine vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		AND_GIVEN("The source and destination registers are different")
		{
			static constexpr uint8_t REG_SRC = Asm::REG_R1;
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			for ( size_t index = 0; index <= 5; ++index )
			{
				WHEN("A bitwise NOT is performed with a reserved bit set")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT) | (1 << index)));

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

		AND_GIVEN("The source and destination registers are the same")
		{
			static constexpr uint8_t REG_SRC = Asm::REG_R0;
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			for ( size_t index = 0; index <= 5; ++index )
			{
				WHEN("A bitwise NOT is performed with a reserved bit set")
				{
					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::NOT) | (1 << index)));

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
}
