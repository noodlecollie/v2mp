#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"
#include "Helpers/EmitterMockDevice.h"

SCENARIO("DPQ: Querying a port's connected state returns the correct answer", "[instructions]")
{
	GIVEN("A virtual machine and a device port")
	{
		static constexpr V2MP_Word PORT_NUMBER = 345;

		TestHarnessVM vm;

		V2MP_DevicePort* port = vm.CreatePort(PORT_NUMBER);
		REQUIRE(port);

		vm.SetR0(PORT_NUMBER);

		AND_GIVEN("The port is not connected to a device")
		{
			REQUIRE_FALSE(V2MP_DevicePort_HasConnectedDevice(port));

			WHEN("The port's connected state is queried")
			{
				REQUIRE(vm.Execute(Asm::DPQ(Asm::DevicePortQuery::CONNECTED)));

				THEN("SR[Z] is set")
				{
					REQUIRE((vm.GetSR() & Asm::SR_Z) != 0);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}
		}

		AND_GIVEN("The port is connected to a device")
		{
			std::shared_ptr<BaseMockDevice> device = vm.ConnectMockDeviceToPort<BaseMockDevice>(PORT_NUMBER);

			REQUIRE(device);
			REQUIRE(V2MP_DevicePort_HasConnectedDevice(port));

			WHEN("The port's connected state is queried")
			{
				REQUIRE(vm.Execute(Asm::DPQ(Asm::DevicePortQuery::CONNECTED)));

				THEN("SR[Z] is not set")
				{
					REQUIRE((vm.GetSR() & Asm::SR_Z) == 0);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}
		}
	}
}

SCENARIO("DPQ: Querying a port's \"readable + not busy\" state returns the correct answer", "[instructions]")
{
	GIVEN("A virtual machine and a device port")
	{
		static constexpr V2MP_Word PORT_NUMBER = 345;

		TestHarnessVM vm;

		V2MP_DevicePort* port = vm.CreatePort(PORT_NUMBER);
		REQUIRE(port);

		vm.SetR0(PORT_NUMBER);

		AND_GIVEN("The port is not connected to a device")
		{
			REQUIRE_FALSE(V2MP_DevicePort_HasConnectedDevice(port));

			WHEN("The port's connected state is queried")
			{
				REQUIRE(vm.Execute(Asm::DPQ(Asm::DevicePortQuery::READABLE_NOT_BUSY)));

				THEN("SR[Z] is set")
				{
					REQUIRE((vm.GetSR() & Asm::SR_Z) != 0);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}
		}

		AND_GIVEN("The port is connected to a device which has no mailbox")
		{
			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_NUMBER);

			REQUIRE(device);
			REQUIRE(V2MP_DevicePort_HasConnectedDevice(port));
			REQUIRE_FALSE(V2MP_DevicePort_HasMailbox(port));
			REQUIRE_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
			REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);

			WHEN("The port's R+NB state is queried")
			{
				REQUIRE(vm.Execute(Asm::DPQ(Asm::DevicePortQuery::READABLE_NOT_BUSY)));

				THEN("SR[Z] is set")
				{
					REQUIRE((vm.GetSR() & Asm::SR_Z) != 0);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}
		}

		AND_GIVEN("The port is connected to a device which leaves the mailbox writeable")
		{
			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_NUMBER);

			REQUIRE(device);
			REQUIRE(V2MP_DevicePort_HasConnectedDevice(port));

			device->AllocateConnectedMailbox(64);

			REQUIRE(device->RelinquishConnectedMailbox());
			REQUIRE(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
			REQUIRE_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
			REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);

			WHEN("The port's R+NB state is queried")
			{
				REQUIRE(vm.Execute(Asm::DPQ(Asm::DevicePortQuery::READABLE_NOT_BUSY)));

				THEN("SR[Z] is set")
				{
					REQUIRE((vm.GetSR() & Asm::SR_Z) != 0);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}
		}

		AND_GIVEN("The port is connected to a device which leaves the mailbox readable")
		{
			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_NUMBER);

			REQUIRE(device);
			REQUIRE(V2MP_DevicePort_HasConnectedDevice(port));

			device->AllocateConnectedMailbox(64);
			device->WriteToConnectedMailbox("Hello");

			REQUIRE(device->RelinquishConnectedMailbox());
			REQUIRE(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
			REQUIRE_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
			REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);

			WHEN("The port's R+NB state is queried")
			{
				REQUIRE(vm.Execute(Asm::DPQ(Asm::DevicePortQuery::READABLE_NOT_BUSY)));

				THEN("SR[Z] is not set")
				{
					REQUIRE((vm.GetSR() & Asm::SR_Z) == 0);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}
		}

		// TODO: Test for busy state
	}
}
