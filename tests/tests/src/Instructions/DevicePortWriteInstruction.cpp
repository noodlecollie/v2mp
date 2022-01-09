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

		AND_GIVEN("A port with an empty mailbox")
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

					AND_THEN("The port's mailbox holds the correct number of bytes")
					{
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
					}

					AND_THEN("R1 holds the length of the entire message")
					{
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_WHEN("The program relinquishes the mailbox")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The port's mailbox is controlled by the device")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
						}

						AND_THEN("The port's mailbox is unavailable")
						{
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
						}

						AND_THEN("The port's mailbox is not busy")
						{
							CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
						}

						AND_THEN("The port's mailbox holds the correct number of bytes")
						{
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
						}

						AND_THEN("R1 still holds the length of the entire message")
						{
							CHECK(vm.GetR1() == sizeof(MESSAGE));
						}

						AND_THEN("The mailbox contains the entire message")
						{
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

					THEN("The port's mailbox is controlled by the program")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
					}

					AND_THEN("The port's mailbox is still writeable")
					{
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);
					}

					AND_THEN("The port's mailbox is busy")
					{
						CHECK(V2MP_DevicePort_IsMailboxBusy(port));
					}

					AND_THEN("The port's mailbox holds the number of bytes transferred this cycle")
					{
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == BYTES_PER_CYCLE);
					}

					AND_THEN("R1 holds the length of the entire message")
					{
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_WHEN("The program relinquishes the mailbox")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The port's mailbox is controlled by the device")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
						}

						AND_THEN("The port's mailbox is unavailable")
						{
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
						}

						AND_THEN("The port's mailbox is not busy")
						{
							CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
						}

						AND_THEN("The port's mailbox holds the correct number of bytes")
						{
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
						}

						AND_THEN("R1 still holds the length of the entire message")
						{
							CHECK(vm.GetR1() == sizeof(MESSAGE));
						}

						THEN("The mailbox contains the entire message")
						{
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

					THEN("The port's mailbox is controlled by the program")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM);
					}

					AND_THEN("The port's mailbox is still writeable")
					{
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);
					}

					AND_THEN("The port's mailbox is busy")
					{
						CHECK(V2MP_DevicePort_IsMailboxBusy(port));
					}

					AND_THEN("The port's mailbox holds the number of bytes transferred this cycle")
					{
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == BYTES_PER_CYCLE);
					}

					AND_THEN("R1 holds the length of the entire message")
					{
						CHECK(vm.GetR1() == sizeof(MESSAGE));
					}

					AND_WHEN("The program relinquishes the mailbox but the write has not completed yet")
					{
						REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::RELINQUISH_MAILBOX, true)));
						REQUIRE_FALSE(vm.CPUHasFault());

						THEN("The port's mailbox is controlled by the device")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
						}

						AND_THEN("The port's mailbox is unavailable")
						{
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
						}

						AND_THEN("The port's mailbox is busy")
						{
							CHECK(V2MP_DevicePort_IsMailboxBusy(port));
						}

						AND_THEN("The port's mailbox holds the correct number of bytes")
						{
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 2 * BYTES_PER_CYCLE);
						}

						AND_THEN("R1 still holds the length of the entire message")
						{
							CHECK(vm.GetR1() == sizeof(MESSAGE));
						}

						AND_WHEN("The program performs enough clock cycles for the write to complete")
						{
							REQUIRE(vm.Execute(Asm::NOP()));
							REQUIRE_FALSE(vm.CPUHasFault());

							REQUIRE(vm.Execute(Asm::NOP()));
							REQUIRE_FALSE(vm.CPUHasFault());

							THEN("The port's mailbox is controlled by the device")
							{
								CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_DEVICE);
							}

							AND_THEN("The port's mailbox is unavailable")
							{
								CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
							}

							AND_THEN("The port's mailbox is not busy")
							{
								CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
							}

							AND_THEN("The port's mailbox holds the correct number of bytes")
							{
								CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
							}

							AND_THEN("R1 still holds the length of the entire message")
							{
								CHECK(vm.GetR1() == sizeof(MESSAGE));
							}

							THEN("The mailbox contains the entire message")
							{
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

// TODO: Reserved bits
// TODO: Status register checks
