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

		AND_GIVEN("A port with an empty mailbox which is the size of the intended message")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<SinkMockDevice> device = vm.ConnectMockDeviceToPort<SinkMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(sizeof(MESSAGE)));
			REQUIRE(device->RelinquishConnectedMailbox());

			AND_GIVEN("The data transfer speed is unlimited")
			{
				device->SetDataTransferSpeed(0);
				REQUIRE(device->DataTransferSpeed() == 0);

				WHEN("A DPO instruction is executed to perform an IDT write to the mailbox")
				{
					vm.SetR0(PORT_ADDRESS);
					vm.SetLR(DS_ADDRESS);
					vm.SetR1(sizeof(MESSAGE));

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The mailbox is written to and becomes exhausted")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED);
						CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_WHEN("The program relinquishes the mailbox")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The mailbox contains the entire message")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
							CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
							CHECK(vm.GetR1() == sizeof(MESSAGE));

							std::vector<V2MP_Byte> outBuffer;
							device->CopyAllDataFromMailbox(outBuffer);

							CHECK(outBuffer.size() == sizeof(MESSAGE));
							CHECK(memcmp(outBuffer.data(), MESSAGE, outBuffer.size()) == 0);
						}
					}
				}
			}

			AND_GIVEN("The data dransfer speed is not fast enough to transfer the entire message in one clock cycle")
			{
				static constexpr size_t BYTES_LEFT_OVER = 2;
				static constexpr size_t BYTES_PER_CYCLE = sizeof(MESSAGE) - BYTES_LEFT_OVER;

				device->SetDataTransferSpeed(BYTES_PER_CYCLE);
				REQUIRE(device->DataTransferSpeed() == BYTES_PER_CYCLE);

				WHEN("A DPO instruction is executed to perform an IDT write to the mailbox")
				{
					vm.SetR0(PORT_ADDRESS);
					vm.SetLR(DS_ADDRESS);
					vm.SetR1(sizeof(MESSAGE));

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The port's mailbox is busy and still writeable")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);
						CHECK(V2MP_DevicePort_IsMailboxBusy(port));
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == BYTES_PER_CYCLE);
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_WHEN("The program relinquishes the mailbox")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The mailbox now contains the entire message")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
							CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
							CHECK(vm.GetR1() == sizeof(MESSAGE));

							std::vector<V2MP_Byte> outBuffer;
							device->CopyAllDataFromMailbox(outBuffer);

							CHECK(outBuffer.size() == sizeof(MESSAGE));
							CHECK(memcmp(outBuffer.data(), MESSAGE, outBuffer.size()) == 0);
						}
					}
				}
			}

			AND_GIVEN("The data dransfer speed is not fast enough to transfer the entire message in two clock cycles")
			{
				static constexpr size_t BYTES_PER_CYCLE = sizeof(MESSAGE) / 4;

				device->SetDataTransferSpeed(BYTES_PER_CYCLE);
				REQUIRE(device->DataTransferSpeed() == BYTES_PER_CYCLE);

				WHEN("A DPO instruction is executed to perform an IDT write to the mailbox")
				{
					vm.SetR0(PORT_ADDRESS);
					vm.SetLR(DS_ADDRESS);
					vm.SetR1(sizeof(MESSAGE));

					REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
					REQUIRE_FALSE(vm.CPUHasFault());

					THEN("The port's mailbox is busy and still writeable")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);
						CHECK(V2MP_DevicePort_IsMailboxBusy(port));
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == BYTES_PER_CYCLE);
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_WHEN("The program relinquishes the mailbox but the write has not completed yet")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The port's mailbox is still busy and still writeable")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
							CHECK(V2MP_DevicePort_IsMailboxBusy(port));
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 2 * BYTES_PER_CYCLE);
							CHECK(vm.GetR1() == sizeof(MESSAGE));
						}

						AND_WHEN("The program performs enough clock cycles for the write to complete")
						{
							REQUIRE(vm.Execute(Asm::NOP()));
							REQUIRE_FALSE(vm.CPUHasFault());

							REQUIRE(vm.Execute(Asm::NOP()));
							REQUIRE_FALSE(vm.CPUHasFault());

							THEN("The mailbox now contains the entire message")
							{
								CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
								CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
								CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
								CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
								CHECK(vm.GetR1() == sizeof(MESSAGE));

								std::vector<V2MP_Byte> outBuffer;
								device->CopyAllDataFromMailbox(outBuffer);

								CHECK(outBuffer.size() == sizeof(MESSAGE));
								CHECK(memcmp(outBuffer.data(), MESSAGE, outBuffer.size()) == 0);
							}
						}
					}
				}
			}
		}
	}
}

SCENARIO("DPO: Attempting to write from a buffer of length zero should raise an IDO fault", "[instructions]")
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

		AND_GIVEN("A port with an empty mailbox")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<SinkMockDevice> device = vm.ConnectMockDeviceToPort<SinkMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(sizeof(MESSAGE)));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to write from a memory buffer of size zero")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(0);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));

				THEN("The CPU contains an IDO fault")
				{
					CHECK(vm.CPUHasFault());
					CHECK(V2MP_CPU_FAULT_CODE(vm.GetCPUFaultWord()) == V2MP_FAULT_IDO);
					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
				}
			}
		}
	}
}

SCENARIO("DPO: Writing to a mailbox should set the status register appropriately", "[instructions]")
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

		AND_GIVEN("A port with an empty mailbox which is the size of the intended message")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<SinkMockDevice> device = vm.ConnectMockDeviceToPort<SinkMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(sizeof(MESSAGE)));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to write the message to the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(sizeof(MESSAGE));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}

		AND_GIVEN("A port with an empty mailbox which is larger than the intended message")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<SinkMockDevice> device = vm.ConnectMockDeviceToPort<SinkMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(sizeof(MESSAGE) + 5));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to write the message to the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(sizeof(MESSAGE));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) == 0);
					CHECK((vm.GetSR() & Asm::SR_C) == 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}

		AND_GIVEN("A port with an empty mailbox which is smaller than the intended message")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<SinkMockDevice> device = vm.ConnectMockDeviceToPort<SinkMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(sizeof(MESSAGE) - 5));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to write the message to the mailbox")
			{
				vm.SetR0(PORT_ADDRESS);
				vm.SetLR(DS_ADDRESS);
				vm.SetR1(sizeof(MESSAGE));

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The status register is set appropriately")
				{
					CHECK((vm.GetSR() & Asm::SR_Z) != 0);
					CHECK((vm.GetSR() & Asm::SR_C) != 0);
					CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
				}
			}
		}
	}
}
