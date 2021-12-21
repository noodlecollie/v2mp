#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word TEST_VALUE = 0xFED5;

SCENARIO("BITW: Performing a bitwise NOT on a register results in the correct value in the register", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(TEST_VALUE);

			WHEN("A bitwise NOT is performed")
			{
				REQUIRE(vm.Execute(Asm::BITWR(REG_DEST, REG_DEST, Asm::BitwiseOp::NOT)));

				THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
				{
					CHECK(vm.GetR0() == static_cast<V2MP_Word>(~TEST_VALUE));
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

			vm.SetR1(TEST_VALUE);

			WHEN("A bitwise NOT is performed")
			{
				REQUIRE(vm.Execute(Asm::BITWR(REG_DEST, REG_DEST, Asm::BitwiseOp::NOT)));

				THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == static_cast<V2MP_Word>(~TEST_VALUE));
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			vm.SetLR(TEST_VALUE);

			WHEN("A bitwise NOT is performed")
			{
				REQUIRE(vm.Execute(Asm::BITWR(REG_DEST, REG_DEST, Asm::BitwiseOp::NOT)));

				THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(~TEST_VALUE));
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			vm.SetPC(TEST_VALUE);

			WHEN("A bitwise NOT is performed")
			{
				REQUIRE(vm.Execute(Asm::BITWR(REG_DEST, REG_DEST, Asm::BitwiseOp::NOT)));

				THEN("The destination register's value should be bitwise inverted, and all other registers should be unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == static_cast<V2MP_Word>(~TEST_VALUE));
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}
