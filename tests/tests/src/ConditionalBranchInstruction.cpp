#include "catch2/catch.hpp"
#include "Helpers/MinimalVirtualMachine.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word BEGIN_ADDRESS = 0xD0D0;
static constexpr V2MP_Word TARGET_ADDRESS = 0x1234;
static constexpr int8_t WORD_INCREMENT = 10;

SCENARIO("CBX: A conditional branch to the address in LR is performed correctly based on the state of SR[Z]", "[instructions]")
{
	GIVEN("A virtual machine with an address set in LR")
	{
		MinimalVirtualMachine vm;

		vm.SetLR(TARGET_ADDRESS);
		vm.SetPC(BEGIN_ADDRESS);

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
					CHECK(vm.GetPC() == BEGIN_ADDRESS);
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
		vm.SetPC(BEGIN_ADDRESS);

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

		AND_GIVEN("SR[C] is not set")
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
					CHECK(vm.GetPC() == BEGIN_ADDRESS);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("CBX: A PC increment or decrement is performed correctly based on the state of SR[Z]", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		vm.SetPC(BEGIN_ADDRESS);

		AND_GIVEN("SR[Z] is set")
		{
			vm.SetSR(Asm::SR_Z);

			WHEN("A conditional increment instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(WORD_INCREMENT)));

				THEN("The value of PC is incremented by the correct number of words, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == static_cast<V2MP_Word>(BEGIN_ADDRESS + (WORD_INCREMENT * sizeof(V2MP_Word))));
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(-WORD_INCREMENT)));

				THEN("The value of PC is decremented by the correct number of words, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == static_cast<V2MP_Word>(BEGIN_ADDRESS - (WORD_INCREMENT * sizeof(V2MP_Word))));
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("SR[Z] is not set")
		{
			vm.SetSR(0);

			WHEN("A conditional increment instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(WORD_INCREMENT)));

				THEN("The value of PC is not modified, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BEGIN_ADDRESS);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(-WORD_INCREMENT)));

				THEN("The value of PC is not modified, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BEGIN_ADDRESS);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("CBX: A PC increment or decrement is performed correctly based on the state of SR[C]", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		vm.SetPC(BEGIN_ADDRESS);

		AND_GIVEN("SR[C] is set")
		{
			vm.SetSR(Asm::SR_C);

			WHEN("A conditional increment instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(WORD_INCREMENT)));

				THEN("The value of PC is incremented by the correct number of words, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == static_cast<V2MP_Word>(BEGIN_ADDRESS + (WORD_INCREMENT * sizeof(V2MP_Word))));
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(-WORD_INCREMENT)));

				THEN("The value of PC is decremented by the correct number of words, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == static_cast<V2MP_Word>(BEGIN_ADDRESS - (WORD_INCREMENT * sizeof(V2MP_Word))));
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}

		AND_GIVEN("SR[C] is not set")
		{
			vm.SetSR(0);

			WHEN("A conditional increment instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(WORD_INCREMENT)));

				THEN("The value of PC is not modified, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BEGIN_ADDRESS);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(-WORD_INCREMENT)));

				THEN("The value of PC is not modified, and all other registers are left unchanged")
				{
					CHECK(vm.GetR0() == 0);
					CHECK(vm.GetR1() == 0);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == BEGIN_ADDRESS);
					CHECK_FALSE(vm.CPUHasFault());
				}
			}
		}
	}
}

SCENARIO("CBX: SR is set appropriately based on whether or not a branch occurred", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		MinimalVirtualMachine vm;

		vm.SetPC(BEGIN_ADDRESS);

		AND_GIVEN("SR[Z] is set")
		{
			vm.SetSR(Asm::SR_Z);

			WHEN("A conditional branch instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZR()));

				THEN("SR[Z] is cleared after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional increment instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(WORD_INCREMENT)));

				THEN("SR[Z] is cleared after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(-WORD_INCREMENT)));

				THEN("SR[Z] is cleared after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}

		AND_GIVEN("SR[Z] is not set")
		{
			vm.SetSR(0);

			WHEN("A conditional branch instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZR()));

				THEN("SR[Z] is set after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional increment instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(WORD_INCREMENT)));

				THEN("SR[Z] is set after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[Z]")
			{
				REQUIRE(vm.Execute(Asm::BXZL(-WORD_INCREMENT)));

				THEN("SR[Z] is set after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}

		AND_GIVEN("SR[C] is set")
		{
			vm.SetSR(Asm::SR_C);

			WHEN("A conditional branch instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCR()));

				THEN("SR[Z] is cleared after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional increment instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(WORD_INCREMENT)));

				THEN("SR[Z] is cleared after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(-WORD_INCREMENT)));

				THEN("SR[Z] is cleared after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}

		AND_GIVEN("SR[C] is not set")
		{
			vm.SetSR(0);

			WHEN("A conditional branch instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCR()));

				THEN("SR[Z] is set after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional increment instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(WORD_INCREMENT)));

				THEN("SR[Z] is set after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A conditional decrement instruction is executed based on the state of SR[C]")
			{
				REQUIRE(vm.Execute(Asm::BXCL(-WORD_INCREMENT)));

				THEN("SR[Z] is set after the instruction")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}
	}
}

SCENARIO("CBX: Setting any literal operand bit if LR is being used as the target address raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
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

		for ( size_t index = 0; index <= 8; ++index )
		{
			WHEN("A conditional branch instruction is executed based on SR[Z], and a reserved bit is set")
			{
				REQUIRE_FALSE(vm.CPUHasFault());
				REQUIRE(vm.Execute(Asm::BXZR() | (1 << index)));

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

			AND_WHEN("A conditional branch instruction is executed based on SR[C], and a reserved bit is set")
			{
				REQUIRE_FALSE(vm.CPUHasFault());
				REQUIRE(vm.Execute(Asm::BXCR() | (1 << index)));

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

SCENARIO("CBX: Executing any instruction with a reserved bit set raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
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

		WHEN("A conditional branch instruction is executed based on SR[Z], and bit 8 is set")
		{
			REQUIRE(vm.Execute(Asm::BXZR() | (1 << 8)));

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

		AND_WHEN("A conditional branch instruction is executed based on SR[Z], and bit 9 is set")
		{
			REQUIRE(vm.Execute(Asm::BXZR() | (1 << 9)));

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

		AND_WHEN("A conditional increment instruction is executed based on SR[Z], and bit 8 is set")
		{
			REQUIRE(vm.Execute(Asm::BXZL(WORD_INCREMENT) | (1 << 8)));

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

		AND_WHEN("A conditional increment instruction is executed based on SR[Z], and bit 9 is set")
		{
			REQUIRE(vm.Execute(Asm::BXZL(WORD_INCREMENT) | (1 << 9)));

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

		AND_WHEN("A conditional branch instruction is executed based on SR[C], and bit 8 is set")
		{
			REQUIRE(vm.Execute(Asm::BXCR() | (1 << 8)));

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

		AND_WHEN("A conditional branch instruction is executed based on SR[C], and bit 9 is set")
		{
			REQUIRE(vm.Execute(Asm::BXCR() | (1 << 9)));

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

		AND_WHEN("A conditional increment instruction is executed based on SR[C], and bit 8 is set")
		{
			REQUIRE(vm.Execute(Asm::BXCL(WORD_INCREMENT) | (1 << 8)));

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

		AND_WHEN("A conditional increment instruction is executed based on SR[C], and bit 9 is set")
		{
			REQUIRE(vm.Execute(Asm::BXCL(WORD_INCREMENT) | (1 << 9)));

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
