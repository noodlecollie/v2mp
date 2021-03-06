#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

SCENARIO("HCF: Executing a halt instruction raises a HCF fault", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		TestHarnessVM_StartsInvalid vm;

		WHEN("A halt instruction is executed")
		{
			REQUIRE(vm.Execute(Asm::HCF()));

			THEN("A HCF fault is raised and register values are unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_HCF);

				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetSR() == 0);
				CHECK(vm.GetIR() == Asm::HCF());
			}
		}

		AND_WHEN("A halt instruction is executed with arguments")
		{
			static constexpr V2MP_Word HALT_ARGS = 0x0DAB;

			REQUIRE(vm.Execute(Asm::HCF(HALT_ARGS)));

			THEN("A HCF fault is raised with the provided arguments, and register values are unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_HCF);
				CHECK(Asm::FaultArgsFromWord(vm.GetCPUFaultWord()) == HALT_ARGS);

				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetSR() == 0);
				CHECK(vm.GetIR() == Asm::HCF(HALT_ARGS));
			}
		}
	}
}
