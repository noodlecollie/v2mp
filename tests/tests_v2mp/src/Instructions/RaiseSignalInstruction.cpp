#include "LibV2MPInternal/Defs.h"
#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "TestUtil/Assembly.h"

static constexpr V2MP_Word VAL_LR = 0xB00B;
static constexpr V2MP_Word VAL_PC = 0x1234;
static constexpr V2MP_Word VAL_SR = 0xFFFF;

static constexpr V2MP_Word INVALID_SIGNAL = 0x9867;
static constexpr V2MP_Word EXIT_CODE = 0xDEAD;

SCENARIO("SIG: Raising \"End Program\" signal causes the program to end", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		TestHarnessVM vm;
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);
		vm.SetSR(VAL_SR);

		AND_GIVEN("The \"End Program\" signal is set up to be raised")
		{
			vm.SetR0(V2MP_SIGNAL_END_PROGRAM);
			vm.SetR1(EXIT_CODE);

			WHEN("The instrution is executed")
			{
				REQUIRE(vm.Execute(Asm::SIG()));

				THEN("The program finishes with the expected exit code, and registers are not modified.")
				{
					CHECK(vm.HasProgramExited());
					CHECK(vm.GetProgramExitCode() == EXIT_CODE);

					CHECK_FALSE(vm.CPUHasFault());
					CHECK(vm.GetR0() == V2MP_SIGNAL_END_PROGRAM);
					CHECK(vm.GetR1() == EXIT_CODE);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK(vm.GetSR() == VAL_SR);
				}
			}
		}
	}
}

SCENARIO("SIG: Attempting to raise an unknown signal raises an INS fault", "[instructions]")
{
	GIVEN("A virtual machine initialised with known values in registers")
	{
		TestHarnessVM vm;
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);
		vm.SetSR(VAL_SR);

		AND_GIVEN("An invalid signal is set up to be raised")
		{
			vm.SetR0(INVALID_SIGNAL);
			vm.SetR1(EXIT_CODE);

			WHEN("The instrution is executed")
			{
				REQUIRE(vm.Execute(Asm::SIG()));

				THEN("An INS fault is raised, and all registers are left unchanged")
				{
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_INS);
					CHECK(vm.GetR0() == INVALID_SIGNAL);
					CHECK(vm.GetR1() == EXIT_CODE);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK(vm.GetSR() == VAL_SR);
				}
			}
		}
	}
}

SCENARIO("SIG: Setting any reserved bit raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		TestHarnessVM vm;

		vm.SetR0(V2MP_SIGNAL_END_PROGRAM);
		vm.SetR1(EXIT_CODE);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);
		vm.SetSR(VAL_SR);

		WHEN("A SIG instruction is executed with a reserved bit set")
		{
			for ( size_t index = 0; index < 12; ++index )
			{
				REQUIRE(vm.Execute(Asm::SIG() | (1 << index)));

				THEN("A RES fault is raised, and all registers are left unchanged")
				{
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
					CHECK(vm.GetR0() == V2MP_SIGNAL_END_PROGRAM);
					CHECK(vm.GetR1() == EXIT_CODE);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK(vm.GetSR() == VAL_SR);
				}
			}
		}
	}
}
