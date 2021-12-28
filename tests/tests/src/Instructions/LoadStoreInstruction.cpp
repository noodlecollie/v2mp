#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word DATA_WORD = 0xF00D;

template<std::size_t N>
static inline bool SetDS(TestHarnessVM& vm, const V2MP_Word (&data)[N])
{
	return vm.SetCSAndDS({0}, data);
}

static inline bool FillDS(TestHarnessVM& vm, V2MP_Word numWords, V2MP_Word fill)
{
	return vm.FillCSAndDS(1, 0, numWords, fill);
}

SCENARIO("LDST: Loading a value from memory places the value in the expected register", "[instructions]")
{
	GIVEN("A virtual machine and known data segment")
	{
		static constexpr V2MP_Word MEM_ADDRESS = 0;

		TestHarnessVM_StartsInvalid vm;
		REQUIRE(SetDS(vm, { DATA_WORD }));

		vm.SetLR(MEM_ADDRESS);

		WHEN("The data is loaded into R0")
		{
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_R0)));

			THEN("The contents of R0 match the data that was in memory")
			{
				CHECK(vm.GetR0() == DATA_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == 0);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("The data is loaded into R1")
		{
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_R1)));

			THEN("The contents of R1 match the data that was in memory")
			{
				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == DATA_WORD);
				CHECK(vm.GetLR() == 0);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("The data is loaded into LR")
		{
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_LR)));

			THEN("The contents of LR match the data that was in memory")
			{
				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == DATA_WORD);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("The data is loaded into PC")
		{
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_PC)));

			THEN("The contents of PC match the data that was in memory")
			{
				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == 0);
				CHECK(vm.GetPC() == DATA_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}
	}
}

SCENARIO("LDST: Loading a value from memory sets the status register appropriately", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word MEM_ADDRESS = 0;

		TestHarnessVM_StartsInvalid vm;

		vm.SetLR(MEM_ADDRESS);

		WHEN("0x0 is loaded from memory")
		{
			REQUIRE(SetDS(vm, { 0x0000 }));
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_R0)));

			THEN("SR[Z] is set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & ~Asm::SR_Z) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1 is loaded from memory")
		{
			REQUIRE(SetDS(vm, { 0x0001 }));
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_R0)));

			THEN("SR[Z] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & ~Asm::SR_Z) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1234 is loaded from memory")
		{
			REQUIRE(SetDS(vm, { 0x1234 }));
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_R0)));

			THEN("SR[Z] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & ~Asm::SR_Z) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}
	}
}

SCENARIO("LDST: Saving a value from memory places the value in the expected memory address", "[instructions]")
{
	GIVEN("A virtual machine and wiped data segment")
	{
		static constexpr V2MP_Word MEM_ADDRESS = 0;

		TestHarnessVM_StartsInvalid vm;

		REQUIRE(FillDS(vm, 1, TestHarnessVM_StartsInvalid::INVALID_WORD));
		vm.SetLR(MEM_ADDRESS);

		WHEN("A data word is written to memory from R0")
		{
			vm.SetR0(DATA_WORD);

			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_R0)));

			THEN("The word in memory matches the value that was in R0")
			{
				V2MP_Word memWord = 0;

				REQUIRE(vm.GetDSWord(MEM_ADDRESS, memWord));
				CHECK(memWord == DATA_WORD);

				CHECK(vm.GetR0() == DATA_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == MEM_ADDRESS);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("A data word is written to memory from R1")
		{
			vm.SetR1(DATA_WORD);

			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_R1)));

			THEN("The word in memory matches the value that was in R1")
			{
				V2MP_Word memWord = 0;

				REQUIRE(vm.GetDSWord(MEM_ADDRESS, memWord));
				CHECK(memWord == DATA_WORD);

				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == DATA_WORD);
				CHECK(vm.GetLR() == MEM_ADDRESS);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("A data word is written to memory from LR")
		{
			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_LR)));

			THEN("The word in memory matches the memory address that was in LR")
			{
				V2MP_Word memWord = 0;

				REQUIRE(vm.GetDSWord(MEM_ADDRESS, memWord));
				CHECK(memWord == MEM_ADDRESS);

				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == MEM_ADDRESS);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("A data word is written to memory from PC")
		{
			vm.SetPC(DATA_WORD);

			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_PC)));

			THEN("The word in memory matches the value that was in PC")
			{
				V2MP_Word memWord = 0;

				REQUIRE(vm.GetDSWord(MEM_ADDRESS, memWord));
				CHECK(memWord == DATA_WORD);

				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == MEM_ADDRESS);
				CHECK(vm.GetPC() == DATA_WORD);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}
	}
}

SCENARIO("LDST: Saving a value to memory sets the status register appropriately", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word MEM_ADDRESS = 0;

		TestHarnessVM_StartsInvalid vm;

		REQUIRE(FillDS(vm, 1, TestHarnessVM_StartsInvalid::INVALID_WORD));
		vm.SetLR(MEM_ADDRESS);

		WHEN("0x0 is saved to memory")
		{
			vm.SetR0({ 0x0000 });
			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_R0)));

			THEN("SR[Z] is set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) != 0);
				CHECK((vm.GetSR() & ~Asm::SR_Z) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1 is loaded from memory")
		{
			vm.SetR0({ 0x0001 });
			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_R0)));

			THEN("SR[Z] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & ~Asm::SR_Z) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}

		WHEN("0x1234 is loaded from memory")
		{
			vm.SetR0({ 0x1234 });
			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_R0)));

			THEN("SR[Z] is not set")
			{
				CHECK((vm.GetSR() & Asm::SR_Z) == 0);
				CHECK((vm.GetSR() & ~Asm::SR_Z) == 0);
				CHECK_FALSE(vm.CPUHasFault());
			}
		}
	}
}

SCENARIO("LDST: Loading or saving a value to an unaligned memory address raises an ALGN fault", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word MEM_ADDRESS = 0x0001;

		TestHarnessVM_StartsInvalid vm;

		REQUIRE(FillDS(vm, 4, 0x0000));

		WHEN("A load is attempted from an unaligned address")
		{
			vm.SetLR(MEM_ADDRESS);
			REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_R0)));

			THEN("An ALGN fault is raised and register values are not modified.")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_ALGN);
				CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == MEM_ADDRESS);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetSR() == 0);
			}
		}

		WHEN("A store is attempted to an unaligned address")
		{
			vm.SetLR(MEM_ADDRESS);
			vm.SetR0(0x1234);

			REQUIRE(vm.Execute(Asm::STOR(Asm::REG_R0)));

			THEN("An ALGN fault is raised and memory is not modified")
			{
				CHECK(vm.CPUHasFault());
				CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_ALGN);
				CHECK(vm.GetR0() == 0x1234);
				CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetLR() == MEM_ADDRESS);
				CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				CHECK(vm.GetSR() == 0);

				V2MP_Word outWord = TestHarnessVM_StartsInvalid::INVALID_WORD;

				REQUIRE(vm.GetDSWord(MEM_ADDRESS - 1, outWord));
				CHECK(outWord == 0x0000);

				REQUIRE(vm.GetDSWord(MEM_ADDRESS + 1, outWord));
				CHECK(outWord == 0x0000);
			}
		}
	}
}

SCENARIO("LDST: Setting any reserved bit raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word MEM_ADDRESS = 0x0000;

		TestHarnessVM_StartsInvalid vm;

		REQUIRE(FillDS(vm, 1, 0x0000));

		for ( size_t index = 0; index <= 8; ++index )
		{
			WHEN("A load is attempted with any reserved bit set")
			{
				vm.ResetCPU();
				vm.SetLR(MEM_ADDRESS);
				REQUIRE(vm.Execute(Asm::LOAD(Asm::REG_R0) | (1 << index)));

				THEN("A RES fault is raised and register values are not modified")
				{
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
					CHECK(vm.GetR0() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetLR() == MEM_ADDRESS);
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetSR() == 0);
				}
			}
		}

		for ( size_t index = 0; index <= 8; ++index )
		{
			WHEN("A store is attempted with any reserved bit set")
			{
				vm.ResetCPU();
				vm.SetLR(MEM_ADDRESS);
				vm.SetR0(0x1234);
				REQUIRE(vm.Execute(Asm::STOR(Asm::REG_R0) | (1 << index)));

				THEN("A RES fault is raised and memory is not modified")
				{
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
					CHECK(vm.GetR0() == 0x1234);
					CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetLR() == MEM_ADDRESS);
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetSR() == 0);

					V2MP_Word outWord = TestHarnessVM_StartsInvalid::INVALID_WORD;

					REQUIRE(vm.GetDSWord(MEM_ADDRESS, outWord));
					CHECK(outWord == 0x0000);
				}
			}
		}
	}
}
