#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"
#include "Helpers/EmitterMockDevice.h"

struct QueryAffirmitive
{
	bool whenDisconnected = false;
	bool WhenConnectedWithNoMailbox = false;

	bool whenMailboxReadableAndNotBusy = false;
	bool whenMailboxReadableAndBusy = false;
	bool whenMailboxExhaustedAfterRead = false;
	bool whenMailboxRelinquishedAfterRead = false;

	bool whenMailboxWriteableAndNotBusy = false;
	bool whenMailboxWriteableAndBusy = false;
	bool whenMailboxExhaustedAfterWrite = false;
	bool whenMailboxRelinquishedAfterWrite = false;
};

static inline bool QueryResultIsAffirmitive(const TestHarnessVM& vm)
{
	return (vm.GetSR() & Asm::SR_Z) == 0;
}

static void TestAllStatesWithQuery(Asm::DevicePortQuery query, const QueryAffirmitive& affirmitive)
{
	GIVEN("A virtual machine and a device port")
	{
		static constexpr V2MP_Word PORT_NUMBER = 345;

		TestHarnessVM vm(2 * sizeof(V2MP_Word));

		V2MP_DevicePort* port = vm.CreatePort(PORT_NUMBER);
		REQUIRE(port);

		AND_GIVEN("The port is not connected to a device")
		{
			REQUIRE_FALSE(V2MP_DevicePort_HasConnectedDevice(port));

			WHEN("The query is executed")
			{
				vm.SetR0(PORT_NUMBER);

				REQUIRE(vm.Execute(Asm::DPQ(query)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The result is correct")
				{
					REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenDisconnected);
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

			WHEN("The query is executed")
			{
				vm.SetR0(PORT_NUMBER);

				REQUIRE(vm.Execute(Asm::DPQ(query)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The result is correct")
				{
					REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.WhenConnectedWithNoMailbox);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}
		}

		AND_GIVEN("The port is connected to a device which leaves the mailbox readable")
		{
			static constexpr const uint8_t MESSAGE[2] = { 3, 1 };

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_NUMBER);

			REQUIRE(device);
			REQUIRE(V2MP_DevicePort_HasConnectedDevice(port));

			device->AllocateConnectedMailbox(2);
			device->WriteToConnectedMailbox(MESSAGE);

			REQUIRE(device->RelinquishConnectedMailbox());
			REQUIRE(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
			REQUIRE_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
			REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);

			WHEN("The query is executed")
			{
				vm.SetR0(PORT_NUMBER);

				REQUIRE(vm.Execute(Asm::DPQ(query)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The result is correct")
				{
					REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxReadableAndNotBusy);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}

			AND_GIVEN("A device port read is in progress")
			{
				vm.FillCSAndDS(1, 0, 1, 0);
				vm.SetR0(PORT_NUMBER);
				vm.SetLR(0);
				vm.SetR1(2);

				device->SetDataTransferSpeed(1);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
				REQUIRE_FALSE(vm.CPUHasFault());
				REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
				REQUIRE(V2MP_DevicePort_IsMailboxBusy(port));

				WHEN("The query is executed")
				{
					vm.SetR0(PORT_NUMBER);

					REQUIRE(vm.Execute(Asm::DPQ(query)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The result is correct")
					{
						REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxReadableAndBusy);
						REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
					}
				}

				AND_GIVEN("The mailbox is exhausted")
				{
					REQUIRE(vm.Execute(Asm::NOP()));
					REQUIRE_FALSE(vm.CPUHasFault());
					REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
					REQUIRE_FALSE(V2MP_DevicePort_IsMailboxBusy(port));

					WHEN("The query is executed")
					{
						vm.SetR0(PORT_NUMBER);

						REQUIRE(vm.Execute(Asm::DPQ(query)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The result is correct")
						{
							REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxExhaustedAfterRead);
							REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
						}
					}

					AND_GIVEN("The mailbox is relinquished by the program")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX)));
						REQUIRE_FALSE(vm.CPUHasFault());
						REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);

						WHEN("The query is executed")
						{
							vm.SetR0(PORT_NUMBER);

							REQUIRE(vm.Execute(Asm::DPQ(query)));
							REQUIRE_FALSE(vm.CPUHasFault());

							THEN("The result is correct")
							{
								REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxRelinquishedAfterRead);
								REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
							}
						}
					}
				}
			}
		}

		AND_GIVEN("The port is connected to a device which leaves the mailbox writeable")
		{
			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_NUMBER);

			REQUIRE(device);
			REQUIRE(V2MP_DevicePort_HasConnectedDevice(port));

			device->AllocateConnectedMailbox(2);

			REQUIRE(device->RelinquishConnectedMailbox());
			REQUIRE(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
			REQUIRE_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
			REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);

			WHEN("The query is executed")
			{
				vm.SetR0(PORT_NUMBER);

				REQUIRE(vm.Execute(Asm::DPQ(query)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The result is correct")
				{
					REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxWriteableAndNotBusy);
					REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
				}
			}

			AND_GIVEN("A device port write is in progress")
			{
				vm.FillCSAndDS(1, 0, 1, 0);
				vm.SetR0(PORT_NUMBER);
				vm.SetLR(0);
				vm.SetR1(2);

				device->SetDataTransferSpeed(1);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
				REQUIRE_FALSE(vm.CPUHasFault());
				REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);
				REQUIRE(V2MP_DevicePort_IsMailboxBusy(port));

				WHEN("The query is executed")
				{
					vm.SetR0(PORT_NUMBER);

					REQUIRE(vm.Execute(Asm::DPQ(query)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The result is correct")
					{
						REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxWriteableAndBusy);
						REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
					}
				}

				AND_GIVEN("The mailbox is exhausted")
				{
					REQUIRE(vm.Execute(Asm::NOP()));
					REQUIRE_FALSE(vm.CPUHasFault());
					REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
					REQUIRE_FALSE(V2MP_DevicePort_IsMailboxBusy(port));

					WHEN("The query is executed")
					{
						vm.SetR0(PORT_NUMBER);

						REQUIRE(vm.Execute(Asm::DPQ(query)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The result is correct")
						{
							REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxExhaustedAfterWrite);
							REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
						}
					}

					AND_GIVEN("The mailbox is relinquished by the program")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX)));
						REQUIRE_FALSE(vm.CPUHasFault());
						REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);

						WHEN("The query is executed")
						{
							vm.SetR0(PORT_NUMBER);

							REQUIRE(vm.Execute(Asm::DPQ(query)));
							REQUIRE_FALSE(vm.CPUHasFault());

							THEN("The result is correct")
							{
								REQUIRE(QueryResultIsAffirmitive(vm) == affirmitive.whenMailboxRelinquishedAfterWrite);
								REQUIRE((vm.GetSR() & ~Asm::SR_Z) == 0);
							}
						}
					}
				}
			}
		}
	}
}

SCENARIO("DPQ: Querying a port's connected state returns the correct answer", "[instructions]")
{
	QueryAffirmitive affirmitive;

	affirmitive.WhenConnectedWithNoMailbox = true;

	affirmitive.whenMailboxReadableAndNotBusy = true;
	affirmitive.whenMailboxReadableAndBusy = true;
	affirmitive.whenMailboxExhaustedAfterRead = true;
	affirmitive.whenMailboxRelinquishedAfterRead = true;

	affirmitive.whenMailboxWriteableAndNotBusy = true;
	affirmitive.whenMailboxWriteableAndBusy = true;
	affirmitive.whenMailboxExhaustedAfterWrite = true;
	affirmitive.whenMailboxRelinquishedAfterWrite = true;

	TestAllStatesWithQuery(Asm::DevicePortQuery::CONNECTED, affirmitive);
}

SCENARIO("DPQ: Querying a port mailbox's \"readable + not busy\" state returns the correct answer", "[instructions]")
{
	QueryAffirmitive affirmitive;

	affirmitive.whenMailboxReadableAndNotBusy = true;

	TestAllStatesWithQuery(Asm::DevicePortQuery::READABLE_NOT_BUSY, affirmitive);
}

SCENARIO("DPQ: Querying a port mailbox's \"writeable + not busy\" state returns the correct answer", "[instructions]")
{
	QueryAffirmitive affirmitive;

	affirmitive.whenMailboxWriteableAndNotBusy = true;

	TestAllStatesWithQuery(Asm::DevicePortQuery::WRITEABLE_NOT_BUSY, affirmitive);
}

SCENARIO("DPQ: Querying a port mailbox's exhausted state returns the correct answer", "[instructions]")
{
	QueryAffirmitive affirmitive;

	affirmitive.whenMailboxExhaustedAfterRead = true;
	affirmitive.whenMailboxExhaustedAfterWrite = true;

	TestAllStatesWithQuery(Asm::DevicePortQuery::EXHAUSTED, affirmitive);
}

SCENARIO("DPQ: Querying a port mailbox's busy state returns the correct answer", "[instructions]")
{
	QueryAffirmitive affirmitive;

	affirmitive.whenMailboxReadableAndBusy = true;
	affirmitive.whenMailboxWriteableAndBusy = true;

	TestAllStatesWithQuery(Asm::DevicePortQuery::BUSY, affirmitive);
}

SCENARIO("DPQ: Querying whether a port's mailbox is controlled by the program returns the correct answer", "[instructions]")
{
	QueryAffirmitive affirmitive;

	affirmitive.whenMailboxReadableAndNotBusy = true;
	affirmitive.whenMailboxReadableAndBusy = true;
	affirmitive.whenMailboxExhaustedAfterRead = true;
	affirmitive.whenMailboxWriteableAndNotBusy = true;
	affirmitive.whenMailboxWriteableAndBusy = true;
	affirmitive.whenMailboxExhaustedAfterWrite = true;

	TestAllStatesWithQuery(Asm::DevicePortQuery::CONTROLLED_BY_PROGRAM, affirmitive);
}
