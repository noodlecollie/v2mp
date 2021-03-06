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

static inline V2MP_Word LEWord(uint8_t b0, uint8_t b1)
{
	return static_cast<V2MP_Word>(b0) | (static_cast<V2MP_Word>(b1) << 8);
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
		TestHarnessVM_StartsInvalid vm(2 * SEGMENT_SIZE_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD);

		REQUIRE(vm.LoadProgram(prog));

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

					THEN("The message is fetched correctly")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
						CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
						CHECK(V2MP_DevicePort_IsMailboxEmpty(port));

						CHECK(vm.GetR0() == PORT_ADDRESS);
						CHECK(vm.GetR1() == sizeof(MESSAGE));
						CHECK(vm.GetLR() == DS_ADDRESS);
						CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);

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

					THEN("The correct number of message bytes are fetched.")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
						CHECK(V2MP_DevicePort_IsMailboxBusy(port));
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE) - BYTES_PER_CYCLE);

						CHECK(vm.GetR0() == PORT_ADDRESS);
						CHECK(vm.GetR1() == sizeof(MESSAGE));
						CHECK(vm.GetLR() == DS_ADDRESS);
						CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);

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

						THEN("The entire message is now fetched")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
							CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
							CHECK(V2MP_DevicePort_IsMailboxEmpty(port));

							CHECK(vm.GetR0() == PORT_ADDRESS);
							CHECK(vm.GetR1() == sizeof(MESSAGE));
							CHECK(vm.GetLR() == DS_ADDRESS);
							CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);

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

SCENARIO("DPO: Performing a DDT read from a device mailbox should transfer the data to program memory", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t MAILBOX_SIZE = 64;

		TestHarnessVM_StartsInvalid vm;

		AND_GIVEN("A port with a message in its mailbox which is more than two bytes long")
		{
			static constexpr const V2MP_Byte MESSAGE[3] = { 1, 2, 3 };

			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to perform a DDT read from the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, false)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("A word of data is fetched from the mailbox")
				{
					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE) - sizeof(V2MP_Word));

					CHECK(vm.GetR0() == PORT_ADDRESS);
					CHECK(vm.GetR1() == sizeof(V2MP_Word));
					CHECK(vm.GetLR() == LEWord(MESSAGE[0], MESSAGE[1]));
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				}
			}
		}

		AND_GIVEN("A port with a message in its mailbox which is exactly two bytes long")
		{
			static constexpr const V2MP_Byte MESSAGE[2] = { 1, 2 };

			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to perform a DDT read from the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, false)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("A word of data is fetched from the mailbox")
				{
					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE) - sizeof(V2MP_Word));

					CHECK(vm.GetR0() == PORT_ADDRESS);
					CHECK(vm.GetR1() == sizeof(V2MP_Word));
					CHECK(vm.GetLR() == LEWord(MESSAGE[0], MESSAGE[1]));
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				}
			}
		}

		AND_GIVEN("A port with a message in its mailbox which is exactly one byte long")
		{
			static constexpr const V2MP_Byte MESSAGE[1] = { 1 };

			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to perform a DDT read from the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, false)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("A word of data is fetched from the mailbox, with the upper byte set to zero")
				{
					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 0);

					CHECK(vm.GetR0() == PORT_ADDRESS);
					CHECK(vm.GetR1() == 1);
					CHECK(vm.GetLR() == LEWord(MESSAGE[0], 0));
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
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
		TestHarnessVM_StartsInvalid vm(2 * SEGMENT_SIZE_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD);

		REQUIRE(vm.LoadProgram(prog));

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

				THEN("A SEG fault is raised and as much of the original message as possible is fetched")
				{
					CHECK(vm.CPUHasFault());
					CHECK(V2MP_CPU_FAULT_CODE(vm.GetCPUFaultWord()) == V2MP_FAULT_SEG);

					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE) - SEGMENT_SIZE_BYTES);

					std::vector<V2MP_Byte> dsData;

					REQUIRE(vm.GetDSData(0, SEGMENT_SIZE_BYTES, dsData));
					CHECK(dsData.size() == SEGMENT_SIZE_BYTES);
					CHECK(memcmp(dsData.data() + DS_ADDRESS, MESSAGE, SEGMENT_SIZE_BYTES - DS_ADDRESS) == 0);
				}
			}
		}
	}
}

SCENARIO("DPO: Relinquishing a readable mailbox should discard any remaining bytes", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 128;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr V2MP_Word DS_ADDRESS = 0;
		static constexpr size_t MAILBOX_SIZE = 64;

		// Enough memory for two segments of equal size.
		TestHarnessVM_StartsInvalid vm(2 * SEGMENT_SIZE_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD);

		REQUIRE(vm.LoadProgram(prog));

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

			WHEN("The port's mailbox is relinquished by the program")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The port's mailbox is controlled by the device")
				{
					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_DEVICE);
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
				}
			}

			AND_GIVEN("Some of the message is read by the device")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(static_cast<V2MP_Word>(sizeof(MESSAGE) / 2));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				WHEN("The port's mailbox is relinquished by the program")
				{
					vm.SetR0(PORT_ADDRESS);

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The port's mailbox is controlled by the device")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_DEVICE);
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
						CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
						CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
					}
				}
			}
		}
	}
}

SCENARIO("DPO: Atempting an IDT read into a buffer of length zero should raise an IDO fault", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 2;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr size_t MAILBOX_SIZE = 64;

		// Enough memory for two segments of equal size.
		TestHarnessVM_StartsInvalid vm(2 * SEGMENT_SIZE_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, 0);

		REQUIRE(vm.LoadProgram(prog));

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

			WHEN("A DPO instruction is executed to read into a zero-sized buffer in memory")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(0);
				vm.SetR1(0);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));

				THEN("The CPU contains an IDO fault")
				{
					CHECK(vm.CPUHasFault());
					CHECK(V2MP_CPU_FAULT_CODE(vm.GetCPUFaultWord()) == V2MP_FAULT_IDO);

					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
				}
			}
		}
	}
}

SCENARIO("DPO: Performing any kind of read from a mailbox that is not readable should raise an IDO fault", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 128;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr V2MP_Word DS_ADDRESS = 0;
		static constexpr size_t MAILBOX_SIZE = 64;

		// Enough memory for two segments of equal size.
		TestHarnessVM_StartsInvalid vm(2 * SEGMENT_SIZE_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD);

		REQUIRE(vm.LoadProgram(prog));

		AND_GIVEN("A port with an empty mailbox")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(device->RelinquishConnectedMailbox());
			REQUIRE(V2MP_DevicePort_IsMailboxEmpty(port));
			REQUIRE(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);

			WHEN("A DPO instruction is executed to perform an IDT read from the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(static_cast<V2MP_Word>(SEGMENT_SIZE_BYTES - DS_ADDRESS));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));

				THEN("The CPU contains an IDO fault")
				{
					CHECK(vm.CPUHasFault());
					CHECK(V2MP_CPU_FAULT_CODE(vm.GetCPUFaultWord()) == V2MP_FAULT_IDO);

					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
				}
			}

			AND_WHEN("A DPO instruction is executed to perform a DDT read from the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, false)));

				THEN("The CPU contains an IDO fault")
				{
					CHECK(vm.CPUHasFault());
					CHECK(V2MP_CPU_FAULT_CODE(vm.GetCPUFaultWord()) == V2MP_FAULT_IDO);

					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
				}
			}
		}
	}
}

SCENARIO("DPO: Performing an IDT read from a mailbox should set the status register appropriately", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 128;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr V2MP_Word DS_ADDRESS = 0;
		static constexpr size_t MAILBOX_SIZE = 64;

		// Enough memory for two segments of equal size.
		TestHarnessVM_StartsInvalid vm(2 * SEGMENT_SIZE_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD);

		REQUIRE(vm.LoadProgram(prog));

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

			WHEN("A DPO instruction is executed to perform an IDT read from the mailbox into a buffer of the exact size of the message")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(static_cast<V2MP_Word>(sizeof(MESSAGE)));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A DPO instruction is executed to perform an IDT read from the mailbox into a buffer larger than the message")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(static_cast<V2MP_Word>(sizeof(MESSAGE) + 5));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A DPO instruction is executed to perform an IDT read from the mailbox into a buffer smaller than the message")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(static_cast<V2MP_Word>(sizeof(MESSAGE) - 5));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}
	}
}

SCENARIO("DPO: Performing a DDT read from a mailbox should set the status register appropriately", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t SEGMENT_SIZE_WORDS = 128;
		static constexpr size_t SEGMENT_SIZE_BYTES = SEGMENT_SIZE_WORDS * sizeof(V2MP_Word);
		static constexpr size_t MAILBOX_SIZE = 64;

		// Enough memory for two segments of equal size.
		TestHarnessVM_StartsInvalid vm(2 * SEGMENT_SIZE_BYTES);

		TestHarnessVM::ProgramDef prog;
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, SEGMENT_SENTRY_WORD);

		REQUIRE(vm.LoadProgram(prog));

		AND_GIVEN("A port with a message in its mailbox")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));

			WHEN("A DDT read is performed from a mailbox containing more than two message bytes")
			{
				static constexpr const V2MP_Byte MESSAGE[3] = { 1, 2, 3 };

				REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
				REQUIRE(device->RelinquishConnectedMailbox());

				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, false)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A DDT read is performed from a mailbox containing exactly two message bytes")
			{
				static constexpr const V2MP_Byte MESSAGE[2] = { 1, 2 };

				REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
				REQUIRE(device->RelinquishConnectedMailbox());

				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, false)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}

			AND_WHEN("A DDT read is performed from a mailbox containing exactly one message byte")
			{
				static constexpr const V2MP_Byte MESSAGE[1] = { 1 };

				REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
				REQUIRE(device->RelinquishConnectedMailbox());

				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::READ, false)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}
	}
}
