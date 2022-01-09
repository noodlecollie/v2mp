#include <cstring>
#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/EmitterMockDevice.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Byte SEGMENT_SENTRY_BYTE = 0xAA;

static constexpr V2MP_Word SEGMENT_SENTRY_WORD =
	(static_cast<V2MP_Word>(SEGMENT_SENTRY_BYTE) << 8) |
	static_cast<V2MP_Word>(SEGMENT_SENTRY_BYTE);

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
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 128;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr V2MP_Word DS_ADDRESS = 0;
		static constexpr size_t MAILBOX_SIZE = 64;

		// Enough memory for two segments of equal size.
		TestHarnessVM vm(2 * SEGMENT_SIZE_BYTES);

		REQUIRE(vm.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD));

		AND_GIVEN("A port with a message in its mailbox")
		{
			static constexpr const char MESSAGE[] = "Mailbox message";

			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
			REQUIRE(device->RelinquishConnectedMailbox());

			AND_GIVEN("The data transfer speed is unlimited")
			{
				device->SetDataTransferSpeed(0);
				REQUIRE(device->DataTransferSpeed() == 0);

				WHEN("A DPO instruction is executed to perform an IDT read from the mailbox")
				{
					vm.SetR0(PORT_ADDRESS);
					vm.SetLR(DS_ADDRESS);
					vm.SetR1(static_cast<V2MP_Word>(SEGMENT_SIZE_BYTES - DS_ADDRESS));

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The port's mailbox is controlled by the program")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
					}

					AND_THEN("The port's mailbox is exhausted")
					{
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
					}

					AND_THEN("The port's mailbox is not busy")
					{
						CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					}

					AND_THEN("The port's mailbox is empty")
					{
						CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
					}

					AND_THEN("R1 holds the length of the entire message")
					{
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_THEN("The DS segment contains the entire message")
					{
						std::vector<V2MP_Byte> dsData;

						REQUIRE(vm.GetDSData(0, SEGMENT_SIZE_BYTES, dsData));
						CHECK(dsData.size() == SEGMENT_SIZE_BYTES);
						CHECK(memcmp(dsData.data() + DS_ADDRESS, MESSAGE, sizeof(MESSAGE)) == 0);

						CHECK(MemoryMatches(
							dsData.data() + DS_ADDRESS + sizeof(MESSAGE),
							SEGMENT_SIZE_BYTES - sizeof(MESSAGE),
							SEGMENT_SENTRY_BYTE
						));
					}
				}
			}

			AND_GIVEN("The data dransfer speed is not fast enough to transfer the entire message in one clock cycle")
			{
				static constexpr size_t BYTES_LEFT_OVER = 2;
				static constexpr size_t BYTES_PER_CYCLE = sizeof(MESSAGE) - BYTES_LEFT_OVER;

				device->SetDataTransferSpeed(BYTES_PER_CYCLE);
				REQUIRE(device->DataTransferSpeed() == BYTES_PER_CYCLE);

				WHEN("A DPO instruction is executed to perform an IDT read from the mailbox")
				{
					vm.SetR0(PORT_ADDRESS);
					vm.SetLR(DS_ADDRESS);
					vm.SetR1(static_cast<V2MP_Word>(SEGMENT_SIZE_BYTES - DS_ADDRESS));

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The port's mailbox is controlled by the program")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
					}

					AND_THEN("The port's mailbox is still readable")
					{
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
					}

					AND_THEN("The port's mailbox is busy")
					{
						CHECK(V2MP_DevicePort_IsMailboxBusy(port));
					}

					AND_THEN("The port's mailbox holds the number of bytes not transferred this cycle")
					{
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE) - BYTES_PER_CYCLE);
					}

					AND_THEN("R1 holds the length of the entire message")
					{
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_THEN("The DS segment contains as many bytes as were transferred in that clock cycle")
					{
						std::vector<V2MP_Byte> dsData;

						REQUIRE(vm.GetDSData(0, SEGMENT_SIZE_BYTES, dsData));
						CHECK(dsData.size() == SEGMENT_SIZE_BYTES);
						CHECK(memcmp(dsData.data() + DS_ADDRESS, MESSAGE, BYTES_PER_CYCLE) == 0);

						CHECK(MemoryMatches(
							dsData.data() + DS_ADDRESS + BYTES_PER_CYCLE,
							SEGMENT_SIZE_BYTES - BYTES_PER_CYCLE,
							SEGMENT_SENTRY_BYTE
						));
					}

					AND_WHEN("A subsequent clock cycle is executed to transfer the rest of the message")
					{
						REQUIRE(vm.Execute(Asm::NOP()));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The port's mailbox is controlled by the program")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
						}

						AND_THEN("The port's mailbox is exhausted")
						{
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
						}

						AND_THEN("The port's mailbox is not busy")
						{
							CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
						}

						AND_THEN("The port's mailbox is empty")
						{
							CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
						}

						AND_THEN("R1 still holds the length of the entire message")
						{
							CHECK(vm.GetR1() == sizeof(MESSAGE));
						}

						AND_THEN("The DS segment contains the entire message")
						{
							std::vector<V2MP_Byte> dsData;

							REQUIRE(vm.GetDSData(0, SEGMENT_SIZE_BYTES, dsData));
							CHECK(dsData.size() == SEGMENT_SIZE_BYTES);
							CHECK(memcmp(dsData.data() + DS_ADDRESS, MESSAGE, sizeof(MESSAGE)) == 0);

							CHECK(MemoryMatches(
								dsData.data() + DS_ADDRESS + sizeof(MESSAGE),
								SEGMENT_SIZE_BYTES - sizeof(MESSAGE),
								SEGMENT_SENTRY_BYTE
							));
						}
					}
				}
			}
		}
	}
}

SCENARIO("DPO: Performing an IDT read from a device mailbox into a memory buffer that's too small should raise a SEG fault", "[instructions]")
{
	GIVEN("A virtual machine with a small amount of memory")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 1;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr V2MP_Word DS_ADDRESS = 0;
		static constexpr size_t MAILBOX_SIZE = 128;

		// Enough memory for two segments of equal size.
		TestHarnessVM vm(2 * SEGMENT_SIZE_BYTES);

		REQUIRE(vm.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD));

		AND_GIVEN("A port with a message in its mailbox")
		{
			static constexpr const char MESSAGE[] = "This message is too large for the DS segment";

			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
			REQUIRE(device->RelinquishConnectedMailbox());

			device->SetDataTransferSpeed(0);
			REQUIRE(device->DataTransferSpeed() == 0);

			WHEN("A DPO instruction is executed to read the entire message from the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(static_cast<V2MP_Word>(sizeof(MESSAGE)));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));

				THEN("The CPU contains a SEG fault")
				{
					CHECK(vm.CPUHasFault());
					CHECK(V2MP_CPU_FAULT_CODE(vm.GetCPUFaultWord()) == V2MP_FAULT_SEG);
				}

				AND_THEN("The port's mailbox is controlled by the program")
				{
					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
				}

				AND_THEN("The port's mailbox is readable")
				{
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
				}

				AND_THEN("The port's mailbox is not busy")
				{
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
				}

				AND_THEN("The port's mailbox holds as many bytes as were not transferred this clock cycle")
				{
					CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE) - SEGMENT_SIZE_BYTES);
				}

				AND_THEN("The data segment contains as much of the original message as would fit")
				{
					std::vector<V2MP_Byte> dsData;

					REQUIRE(vm.GetDSData(0, SEGMENT_SIZE_BYTES, dsData));
					CHECK(dsData.size() == SEGMENT_SIZE_BYTES);
					CHECK(memcmp(dsData.data() + DS_ADDRESS, MESSAGE, SEGMENT_SIZE_BYTES - DS_ADDRESS) == 0);
				}
			}
		}
	}
}

// TODO: Reserved bits
// TODO: Status register checks
