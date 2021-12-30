#include <cstring>
#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

static bool MemoryMatches(const V2MP_Byte* data, size_t length, V2MP_Byte check)
{
	while ( length > 0 )
	{
		if ( *data != check )
		{
			return false;
		}

		++data;
		--length;
	}

	return true;
}

SCENARIO("DPO: Performing an IDT read from a device mailbox should transfer the data to program memory", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static V2MP_Word PORT_ADDRESS = 1234;
		static size_t SEGMENT_SIZE_WORDS = 128;
		static V2MP_Word DS_ADDRESS = 0;

		TestHarnessVM vm(2 * SEGMENT_SIZE_WORDS);

		vm.FillCSAndDS(1, 0, SEGMENT_SIZE_WORDS, 0xAAAA);

		AND_GIVEN("A port with a message in its mailbox")
		{
			static constexpr const char MESSAGE[] = "Mailbox message";

			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS, 64);
			REQUIRE(port);

			REQUIRE(vm.WriteToPortMailbox(PORT_ADDRESS, MESSAGE) == sizeof(MESSAGE));

			WHEN("A DPO instruction is executed to perform an IDT read from the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(SEGMENT_SIZE_WORDS * sizeof(V2MP_Word));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The DS segment should contain this data")
				{
					std::vector<V2MP_Byte> dsData;

					REQUIRE(vm.GetDSData(0, SEGMENT_SIZE_WORDS * sizeof(V2MP_Word), dsData));
					CHECK(dsData.size() == SEGMENT_SIZE_WORDS * sizeof(V2MP_Word));
					CHECK(memcmp(dsData.data() + DS_ADDRESS, MESSAGE, sizeof(MESSAGE)) == 0);

					CHECK(MemoryMatches(
						dsData.data() + DS_ADDRESS + sizeof(MESSAGE),
						(SEGMENT_SIZE_WORDS * sizeof(V2MP_Word)) - sizeof(MESSAGE),
						0xAA
					));
				}

				AND_THEN("R1 should hold the length of the message that was in the mailbox")
				{
					CHECK(vm.GetR1() == sizeof(MESSAGE));
				}
			}
		}
	}
}
