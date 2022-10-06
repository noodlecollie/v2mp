#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word VAL_R0 = 0x0101;
static constexpr V2MP_Word VAL_R1 = 0xBEEF;
static constexpr V2MP_Word VAL_LR = 0xB00B;
static constexpr V2MP_Word VAL_PC = 0x1234;
static constexpr V2MP_Word VAL_SR = 0xFFFF;

SCENARIO("NOP: Executing instruction does not modify values in registers", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);
		vm.SetSR(VAL_SR);

		WHEN("The instrution is executed")
		{
			REQUIRE(vm.Execute(Asm::NOP()));

			THEN("The values placed into registers are correct")
			{
				CHECK_FALSE(vm.CPUHasFault());
				CHECK(vm.GetR0() == VAL_R0);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
				CHECK(vm.GetSR() == VAL_SR);
			}
		}
	}
}

SCENARIO("NOP: Setting any reserved bit raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);
		vm.SetSR(VAL_SR);

		WHEN("A NOP instruction is executed with a reserved bit set")
		{
			for ( size_t index = 0; index < 12; ++index )
			{
				REQUIRE(vm.Execute(Asm::NOP() | (1 << index)));

				THEN("A RES fault is raised, and all registers are left unchanged")
				{
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK(vm.GetSR() == VAL_SR);
				}
			}
		}
	}
}
