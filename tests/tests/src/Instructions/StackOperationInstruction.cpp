#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

SCENARIO("STK: Pushing registers should result in the correct values being placed onto the stack", "[instructions]")
{
	GIVEN("A virtual machine with values in registers")
	{
		static constexpr size_t TOTAL_MEMORY_BYTES = 128 * sizeof(V2MP_Word);
		static constexpr size_t STACK_SIZE_WORDS = 16;

		static constexpr V2MP_Word VAL_R0 = 0;
		static constexpr V2MP_Word VAL_R1 = 1;
		static constexpr V2MP_Word VAL_LR = 2;
		static constexpr V2MP_Word VAL_PC = 3;

		TestHarnessVM vm(TOTAL_MEMORY_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCS(1, 0);
		prog.SetStackSize(STACK_SIZE_WORDS);

		REQUIRE(vm.LoadProgram(prog));

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("All four registers are pushed onto the stack")
		{
			REQUIRE(vm.Execute(Asm::PUSH(
				(1 << V2MP_REGID_R0) |
				(1 << V2MP_REGID_R1) |
				(1 << V2MP_REGID_LR) |
				(1 << V2MP_REGID_PC)
			)));

			THEN("The stack memory contains these values in the correct order")
			{
				REQUIRE(vm.GetSP() == 4 * sizeof(V2MP_Word));

				std::vector<V2MP_Word> data;
				REQUIRE(vm.GetSSData(vm.GetSP() - (4 * sizeof(V2MP_Word)), 4, data));

				CHECK(data[0] == VAL_R0);
				CHECK(data[1] == VAL_R1);
				CHECK(data[2] == VAL_LR);
				CHECK(data[3] == VAL_PC);
			}
		}
	}
}

// TODO: Ensure SR stays the same after instruction
