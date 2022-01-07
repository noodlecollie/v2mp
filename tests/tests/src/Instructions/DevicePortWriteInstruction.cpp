#include <vector>
#include <cstring>
#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/SinkMockDevice.h"
#include "Helpers/Assembly.h"

SCENARIO("DPO: Performing an IDT write from program memory should transfer the data to a mailbox", "[instructions]")
{
	GIVEN("A virtual machine with a message in DS")
	{
		static constexpr const char MESSAGE[] = "Mailbox message";
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 128;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr V2MP_Word DS_ADDRESS = 0;

		// Enough memory for two segments of equal size.
		TestHarnessVM vm(2 * SEGMENT_SIZE_BYTES);

		std::vector<V2MP_Word> cs;
		cs.resize(1, 0);

		std::vector<V2MP_Word> ds;
		ds.resize(sizeof(MESSAGE));
		memcpy(ds.data(), MESSAGE, ds.size());

		REQUIRE(vm.SetCSAndDS(cs, ds));

		AND_GIVEN("A port with am empty mailbox")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<SinkMockDevice> device = vm.ConnectMockDeviceToPort<SinkMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(64));
			REQUIRE(device->RelinquishConnectedMailbox());

			AND_GIVEN("The data transfer speed is unlimited")
			{
				device->SetDataTransferSpeed(0);
				REQUIRE(device->DataTransferSpeed() == 0);

				WHEN("A DPO instruction is executed to perform an IDT read from the mailbox")
				{
					vm.SetR0(PORT_ADDRESS);
					vm.SetLR(DS_ADDRESS);
					vm.SetR1(sizeof(MESSAGE));

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The mailbox contains the entire message")
					{
						std::vector<V2MP_Byte> outBuffer;
						device->ReadAllDataFromMailbox(outBuffer);

						REQUIRE(outBuffer.size() == sizeof(MESSAGE));
					}
				}
			}
		}
	}
}
