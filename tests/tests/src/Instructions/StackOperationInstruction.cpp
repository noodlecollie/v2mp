#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

struct STKResult
{
	bool r0Push = false;
	bool r1Push = false;
	bool lrPush = false;
	bool pcPush = false;
	bool srWasSameAfterPush = false;

	bool r0Pop = false;
	bool r1Pop = false;
	bool lrPop = false;
	bool pcPop = false;
	bool srWasSameAfterPop = false;

	inline void SetPushResult(size_t index, bool result)
	{
		switch ( index )
		{
			case V2MP_REGID_R0:
			{
				r0Push = result;
				break;
			}

			case V2MP_REGID_R1:
			{
				r1Push = result;
				break;
			}

			case V2MP_REGID_LR:
			{
				lrPush = result;
				break;
			}

			case V2MP_REGID_PC:
			{
				pcPush = result;
				break;
			}

			default:
			{
				break;
			}
		}
	}

	inline void SetPopResult(size_t index, bool result)
	{
		switch ( index )
		{
			case V2MP_REGID_R0:
			{
				r0Pop = result;
				break;
			}

			case V2MP_REGID_R1:
			{
				r1Pop = result;
				break;
			}

			case V2MP_REGID_LR:
			{
				lrPop = result;
				break;
			}

			case V2MP_REGID_PC:
			{
				pcPop = result;
				break;
			}

			default:
			{
				break;
			}
		}
	}
};

static void PerformSTKTest(size_t iteration, STKResult& result)
{
	static constexpr size_t TOTAL_MEMORY_BYTES = 128 * sizeof(V2MP_Word);
	static constexpr size_t STACK_SIZE_WORDS = 16;

	static constexpr V2MP_Word VAL_R0 = 1;
	static constexpr V2MP_Word VAL_R1 = 2;
	static constexpr V2MP_Word VAL_LR = 3;
	static constexpr V2MP_Word VAL_PC = 4;

	const V2MP_Word ORIG_VALUES[4] = { VAL_R0, VAL_R1, VAL_LR, VAL_PC };

	TestHarnessVM vm(TOTAL_MEMORY_BYTES);

	TestHarnessVM::ProgramDef prog;
	prog.FillCS(1, 0);
	prog.SetStackSize(STACK_SIZE_WORDS);

	if ( !vm.LoadProgram(prog) )
	{
		throw std::runtime_error("Could not load program");
	}

	const V2MP_Word regFlags = static_cast<V2MP_Word>(iteration & 0xF);

	size_t numRegisters = 0;

	for ( size_t index = 0; index <= V2MP_REGID_MAX; ++index )
	{
		if ( regFlags & (1 << index) )
		{
			++numRegisters;
		}
	}

	if ( numRegisters > 4 )
	{
		throw std::runtime_error("Unexpected number of registers");
	}

	vm.SetR0(VAL_R0);
	vm.SetR1(VAL_R1);
	vm.SetLR(VAL_LR);
	vm.SetPC(VAL_PC);

	const V2MP_Word srPrePush = vm.GetSR();

	if ( !vm.Execute(Asm::PUSH(regFlags)) )
	{
		throw std::runtime_error("PUSH instruction failed to execute");
	}

	if ( vm.GetSP() != numRegisters * sizeof(V2MP_Word) )
	{
		throw std::runtime_error("Stack pointer value was incorrect after push");
	}

	std::vector<V2MP_Word> data;

	if ( !vm.GetSSData(vm.GetSP() - (numRegisters * sizeof(V2MP_Word)), numRegisters, data) )
	{
		throw std::runtime_error("Could not get stack data");
	}

	size_t stackWordsChecked = 0;

	for ( size_t index = 0; index <= V2MP_REGID_MAX; ++index )
	{
		if ( regFlags & (1 << index) )
		{
			result.SetPushResult(index, data[stackWordsChecked++] == ORIG_VALUES[index]);
		}
		else
		{
			result.SetPushResult(index, vm.GetRegisterValue(static_cast<V2MP_RegisterIndex>(index)));
		}
	}

	result.srWasSameAfterPush = srPrePush == vm.GetSR();

	vm.SetR0(0);
	vm.SetR1(0);
	vm.SetLR(0);
	vm.SetPC(0);

	const V2MP_Word srPrePop = vm.GetSR();

	if ( !vm.Execute(Asm::POP(regFlags)) )
	{
		throw std::runtime_error("POP instruction failed to execute");
	}

	if ( vm.GetSP() != 0 )
	{
		throw std::runtime_error("Stack pointer value was incorrect after push");
	}

	result.SetPopResult(V2MP_REGID_R0, vm.GetR0() == ((regFlags & (1 << V2MP_REGID_R0)) ? VAL_R0 : 0));
	result.SetPopResult(V2MP_REGID_R1, vm.GetR1() == ((regFlags & (1 << V2MP_REGID_R1)) ? VAL_R1 : 0));
	result.SetPopResult(V2MP_REGID_LR, vm.GetLR() == ((regFlags & (1 << V2MP_REGID_LR)) ? VAL_LR : 0));
	result.SetPopResult(V2MP_REGID_PC, vm.GetPC() == ((regFlags & (1 << V2MP_REGID_PC)) ? VAL_PC : 0));

	result.srWasSameAfterPop = srPrePop == vm.GetSR();
}

SCENARIO("STK: Pushing and popping registers should result in the correct values being placed onto or removed from the stack", "[instructions]")
{
	std::vector<STKResult> results;
	results.resize(16);

	size_t iteration = 1;

	// For some reason this test really didn't like the use of Catch clauses in loops,
	// so it's been coerced into something more iterative but really stupid-looking.

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 1
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 2
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 3
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 4
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 5
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 6
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 7
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 8
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 9
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 10
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 11
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 12
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 13
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 14
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
	++iteration;

	REQUIRE_NOTHROW(PerformSTKTest(iteration, results[iteration]));	// 15
	CHECK(results[iteration].r0Push);
	CHECK(results[iteration].r0Pop);
	CHECK(results[iteration].r1Push);
	CHECK(results[iteration].r1Pop);
	CHECK(results[iteration].lrPush);
	CHECK(results[iteration].lrPop);
	CHECK(results[iteration].pcPush);
	CHECK(results[iteration].pcPop);
	CHECK(results[iteration].srWasSameAfterPush);
	CHECK(results[iteration].srWasSameAfterPop);
}

SCENARIO("STK: Performing a stack operation with reserved bits set should raise a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0001;
		static constexpr V2MP_Word VAL_R1 = 0x0002;
		static constexpr V2MP_Word VAL_LR = 0x0003;
		static constexpr V2MP_Word VAL_PC = 0x0004;

		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		for ( size_t index = 4; index <= 10; ++index )
		{
			WHEN("A push is performed with a reserved bit set")
			{
				REQUIRE(vm.Execute(Asm::PUSH(V2MP_REGID_R1) | (1 << index)));

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
					CHECK(vm.GetSP() == 0);
				}
			}

			AND_WHEN("A pop is performed with a reserved bit set")
			{
				REQUIRE(vm.Execute(Asm::POP(V2MP_REGID_R1) | (1 << index)));

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
					CHECK(vm.GetSP() == 0);
				}
			}
		}

		AND_WHEN("A push is performed with no registers specified")
		{
			REQUIRE(vm.Execute(Asm::PUSH(0)));

			THEN("A RES fault is raised, and all registers are left unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
				CHECK(vm.GetR0() == VAL_R0);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
				CHECK(vm.GetSR() == 0);
				CHECK(vm.GetSP() == 0);
			}
		}

		AND_WHEN("A pop is performed with no registers specified")
		{
			REQUIRE(vm.Execute(Asm::POP(0)));

			THEN("A RES fault is raised, and all registers are left unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
				CHECK(vm.GetR0() == VAL_R0);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
				CHECK(vm.GetSR() == 0);
				CHECK(vm.GetSP() == 0);
			}
		}
	}
}

SCENARIO("STK: Performing a stack push that exceeds the stack space raises a SOF fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0001;
		static constexpr V2MP_Word VAL_R1 = 0x0002;
		static constexpr V2MP_Word VAL_LR = 0x0003;
		static constexpr V2MP_Word VAL_PC = 0x0004;

		TestHarnessVM vm(4);

		TestHarnessVM::ProgramDef prog;
		prog.FillCS(1, 0);
		prog.SetStackSize(1);

		REQUIRE(vm.LoadProgram(prog));

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("A stack push is performed that would exceed the available stack space")
		{
			REQUIRE(vm.Execute(Asm::PUSH((1 << V2MP_REGID_R0) | (1 << V2MP_REGID_R1))));

			THEN("A SOF fault is raised and all registers are left unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_SOF);
				CHECK(vm.GetR0() == VAL_R0);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
				CHECK(vm.GetSR() == 0);
				CHECK(vm.GetSP() == 0);
			}
		}
	}
}

SCENARIO("STK: Performing a stack pop that exceeds the available stack words raises a SOF fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0001;
		static constexpr V2MP_Word VAL_R1 = 0x0002;
		static constexpr V2MP_Word VAL_LR = 0x0003;
		static constexpr V2MP_Word VAL_PC = 0x0004;

		TestHarnessVM vm(4);

		TestHarnessVM::ProgramDef prog;
		prog.FillCS(1, 0);
		prog.SetStackSize(1);

		REQUIRE(vm.LoadProgram(prog));

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		WHEN("A stack pop is performed that would exceed the available words on the stack")
		{
			REQUIRE(vm.Execute(Asm::PUSH(1 << V2MP_REGID_R0)));
			REQUIRE(vm.Execute(Asm::POP((1 << V2MP_REGID_R0) | (1 << V2MP_REGID_R1))));

			THEN("A SOF fault is raised and all registers are left unchanged")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_SOF);
				CHECK(vm.GetR0() == VAL_R0);
				CHECK(vm.GetR1() == VAL_R1);
				CHECK(vm.GetLR() == VAL_LR);
				CHECK(vm.GetPC() == VAL_PC);
				CHECK(vm.GetSR() == 0);
				CHECK(vm.GetSP() == 2);
			}
		}
	}
}
