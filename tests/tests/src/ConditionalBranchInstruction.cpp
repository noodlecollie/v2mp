#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word TARGET_ADDRESS = 0x1234;

SCENARIO("CBX: A conditional branch to the address in LR is performed correctly based on the state of SR[Z]", "[instructions]")
{
	GIVEN("A virtual machine with an address set in LR")
	{
		MinimalVirtualMachine vm;

		vm.SetLR(TARGET_ADDRESS);

		AND_GIVEN("SR[Z] is set")
		{
			vm.SetSR(Asm::SR_Z);

			WHEN("A conditional branch instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZR()));

				THEN("The value of PC is set to the target address, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == TARGET_ADDRESS);
					CHECK(vm.GetPC() == TARGET_ADDRESS);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("SR[Z] is not set")
		{
			vm.SetSR(0);

			WHEN("A conditional branch instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZR()));

				THEN("The value of PC is not modified, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == TARGET_ADDRESS);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("CBX: A conditional branch to the address in LR is performed correctly based on the state of SR[C]", "[instructions]")
{
	GIVEN("A virtual machine with an address set in LR")
	{
		MinimalVirtualMachine vm;

		vm.SetLR(TARGET_ADDRESS);

		AND_GIVEN("SR[C] is set")
		{
			vm.SetSR(Asm::SR_C);

			WHEN("A conditional branch instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCR()));

				THEN("The value of PC is set to the target address, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == TARGET_ADDRESS);
					CHECK(vm.GetPC() == TARGET_ADDRESS);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("SR[Z] is not set")
		{
			vm.SetSR(0);

			WHEN("A conditional branch instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCR()));

				THEN("The value of PC is not modified, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == TARGET_ADDRESS);
					CHECK(vm.GetPC() == 0);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

// TODO:
// - Tests of increment and decrement based on SR[Z] and SR[C]
// - Tests of state of SR after instruction
// - Tests for reserved bits
