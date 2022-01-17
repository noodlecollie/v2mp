#include "catch2/catch.hpp"
#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Modules/DeviceCollection.h"
#include "V2MPInternal/Modules/Device.h"

SCENARIO("Creating and destroying devices and ports automatically disconnects the relevant devices from their ports", "[device_ports]")
{
	GIVEN("A device collection and device port collection")
	{
		V2MP_DeviceCollection* dc = V2MP_DeviceCollection_AllocateAndInit();
		REQUIRE(dc);

		V2MP_DevicePortCollection* dpc = V2MP_DevicePortCollection_AllocateAndInit();
		REQUIRE(dpc);

		AND_GIVEN("A device and device port")
		{
			static constexpr V2MP_Word PORT_ADDRESS = 1234;

			V2MP_Device* device = V2MP_DeviceCollection_CreateDevice(dc);
			REQUIRE(device);
			REQUIRE(V2MP_DeviceCollection_GetDeviceCount(dc) == 1);

			V2MP_DevicePort* port = V2MP_DevicePortCollection_CreatePort(dpc, PORT_ADDRESS);
			REQUIRE(port);

			WHEN("The device is connected to the port")
			{
				REQUIRE(V2MP_DevicePort_ConnectDevice(port, device));

				THEN("The port reports that the device is connected")
				{
					CHECK(V2MP_DevicePort_HasConnectedDevice(port));
					CHECK(V2MP_DevicePort_GetConnectedDevice(port) == device);
				}

				AND_WHEN("The device is subsequently disconnected from the port")
				{
					REQUIRE(V2MP_DevicePort_DisconnectDevice(port));

					THEN("The port reports that no device is connected")
					{
						CHECK_FALSE(V2MP_DevicePort_HasConnectedDevice(port));
						CHECK(V2MP_DevicePort_GetConnectedDevice(port) == nullptr);
						CHECK(V2MP_DevicePort_GetConnectedDevice(port) != device);
					}
				}

				AND_WHEN("The device is destroyed")
				{
					REQUIRE(V2MP_DeviceCollection_DestroyDevice(dc, device));
					device = nullptr;

					REQUIRE(V2MP_DeviceCollection_GetDeviceCount(dc) == 0);

					THEN("The port reports that no device is connected")
					{
						CHECK_FALSE(V2MP_DevicePort_HasConnectedDevice(port));
						CHECK(V2MP_DevicePort_GetConnectedDevice(port) == nullptr);
					}
				}

				AND_WHEN("The device collection is destroyed")
				{
					V2MP_DeviceCollection_DeinitAndFree(dc);
					dc = nullptr;
					device = nullptr;

					THEN("The port reports that no device is connected")
					{
						CHECK_FALSE(V2MP_DevicePort_HasConnectedDevice(port));
						CHECK(V2MP_DevicePort_GetConnectedDevice(port) == nullptr);
					}
				}

				AND_WHEN("The port is destroyed")
				{
					REQUIRE(V2MP_DevicePortCollection_DestroyPort(dpc, PORT_ADDRESS));
					port = nullptr;

					THEN("The device reports that it is not connected to a port")
					{
						CHECK_FALSE(V2MP_Device_IsConnectedToPort(device));
					}
				}

				AND_WHEN("The port collection is destroyed")
				{
					V2MP_DevicePortCollection_DeinitAndFree(dpc);
					dpc = nullptr;
					port = nullptr;

					THEN("The device reports that it is not connected to a port")
					{
						CHECK_FALSE(V2MP_Device_IsConnectedToPort(device));
					}
				}
			}

			V2MP_DevicePortCollection_DestroyPort(dpc, PORT_ADDRESS);
			V2MP_DeviceCollection_DestroyDevice(dc, device);
		}

		V2MP_DevicePortCollection_DeinitAndFree(dpc);
		V2MP_DeviceCollection_DeinitAndFree(dc);
	}
}

SCENARIO("Device port mailbox state", "[device_ports]")
{
	GIVEN("A device collection, device port collection, and device port")
	{
		static constexpr V2MP_Word PORT_ADDRESS = 1234;
		static constexpr size_t MAILBOX_SIZE = 64;
		static constexpr const char MAILBOX_MESSAGE[] = "This is a test message";

		V2MP_DeviceCollection* dc = V2MP_DeviceCollection_AllocateAndInit();
		REQUIRE(dc);

		V2MP_DevicePortCollection* dpc = V2MP_DevicePortCollection_AllocateAndInit();
		REQUIRE(dpc);

		V2MP_DevicePort* port = V2MP_DevicePortCollection_CreatePort(dpc, PORT_ADDRESS);
		REQUIRE(port);

		WHEN("The port is not connected to any device")
		{
			REQUIRE_FALSE(V2MP_DevicePort_HasConnectedDevice(port));

			THEN("The mailbox is still considered controlled by a device")
			{
				CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_DEVICE);
				CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
				CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
				CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
				CHECK_FALSE(V2MP_DevicePort_IsMailboxFull(port));
				CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 0);
				CHECK(V2MP_DevicePort_MailboxBytesFree(port) == 0);
			}
		}

		AND_WHEN("The port is connected to a device")
		{
			V2MP_Device* device = V2MP_DeviceCollection_CreateDevice(dc);
			REQUIRE(device);

			REQUIRE(V2MP_DevicePort_ConnectDevice(port, device));
			REQUIRE(V2MP_DevicePort_GetConnectedDevice(port) == device);
			REQUIRE(V2MP_Device_IsConnectedToPort(device));
			REQUIRE(V2MP_Device_AddressOfConnectedPort(device) == PORT_ADDRESS);

			THEN("The mailbox is considered controlled by the device")
			{
				CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_DEVICE);
				CHECK(V2MP_Device_ControlsConnectedMailbox(device));
				CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
				CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
				CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
				CHECK_FALSE(V2MP_DevicePort_IsMailboxFull(port));
				CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 0);
				CHECK(V2MP_DevicePort_MailboxBytesFree(port) == 0);
				CHECK(V2MP_Device_IsConnectedMailboxEmpty(device));
				CHECK_FALSE(V2MP_Device_IsConnectedMailboxFull(device));
				CHECK(V2MP_Device_UsedBytesInConnectedMailbox(device) == 0);
				CHECK(V2MP_Device_FreeBytesInConnectedMailbox(device) == 0);
			}

			AND_WHEN("The device tries to write to the mailbox before it has been allocated")
			{
				const size_t written = V2MP_Device_WriteToConnectedMailbox(
					device,
					reinterpret_cast<const V2MP_Byte*>(MAILBOX_MESSAGE),
					sizeof(MAILBOX_MESSAGE)
				);

				THEN("No bytes are written to the mailbox")
				{
					CHECK(written == 0);
				}
			}

			AND_WHEN("The device allocates a mailbox")
			{
				REQUIRE(V2MP_Device_AllocateConnectedMailbox(device, MAILBOX_SIZE));

				THEN("The mailbox is considered controlled by the device")
				{
					CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_DEVICE);
					CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
					CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_UNAVAILABLE);
					CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
					CHECK_FALSE(V2MP_DevicePort_IsMailboxFull(port));
					CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 0);
					CHECK(V2MP_DevicePort_MailboxBytesFree(port) == MAILBOX_SIZE);
					CHECK(V2MP_Device_IsConnectedMailboxEmpty(device));
					CHECK_FALSE(V2MP_Device_IsConnectedMailboxFull(device));
					CHECK(V2MP_Device_UsedBytesInConnectedMailbox(device) == 0);
					CHECK(V2MP_Device_FreeBytesInConnectedMailbox(device) == MAILBOX_SIZE);
				}

				AND_WHEN("The device writes to the mailbox")
				{
					const size_t written = V2MP_Device_WriteToConnectedMailbox(
						device,
						reinterpret_cast<const V2MP_Byte*>(MAILBOX_MESSAGE),
						sizeof(MAILBOX_MESSAGE)
					);

					AND_THEN("The mailbox is considered partially filled")
					{
						CHECK(written == sizeof(MAILBOX_MESSAGE));
						CHECK_FALSE(V2MP_DevicePort_IsMailboxEmpty(port));
						CHECK_FALSE(V2MP_DevicePort_IsMailboxFull(port));
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MAILBOX_MESSAGE));
						CHECK(V2MP_DevicePort_MailboxBytesFree(port) == MAILBOX_SIZE - sizeof(MAILBOX_MESSAGE));
						CHECK_FALSE(V2MP_Device_IsConnectedMailboxEmpty(device));
						CHECK(V2MP_Device_UsedBytesInConnectedMailbox(device) == sizeof(MAILBOX_MESSAGE));
						CHECK(V2MP_Device_FreeBytesInConnectedMailbox(device) == MAILBOX_SIZE - sizeof(MAILBOX_MESSAGE));
					}

					AND_WHEN("The device relinquishes the mailbox")
					{
						REQUIRE(V2MP_Device_RelinquishConnectedMailbox(device));

						THEN("The mailbox is considered controlled by the program")
						{
							CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
							CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
							CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE);
							CHECK_FALSE(V2MP_DevicePort_IsMailboxEmpty(port));
							CHECK_FALSE(V2MP_DevicePort_IsMailboxFull(port));
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MAILBOX_MESSAGE));
							CHECK(V2MP_DevicePort_MailboxBytesFree(port) == MAILBOX_SIZE - sizeof(MAILBOX_MESSAGE));
							CHECK_FALSE(V2MP_Device_IsConnectedMailboxEmpty(device));
							CHECK_FALSE(V2MP_Device_IsConnectedMailboxFull(device));
							CHECK(V2MP_Device_UsedBytesInConnectedMailbox(device) == sizeof(MAILBOX_MESSAGE));
							CHECK(V2MP_Device_FreeBytesInConnectedMailbox(device) == MAILBOX_SIZE - sizeof(MAILBOX_MESSAGE));
						}

						AND_WHEN("The device relinquishes the mailbox without having control of it")
						{
							const bool secondRelinquish = V2MP_Device_RelinquishConnectedMailbox(device);

							THEN("The result of the function should be false")
							{
								CHECK_FALSE(secondRelinquish);
							}
						}
					}
				}

				AND_WHEN("The device relinquishes the mailbox without having written to it")
				{
					REQUIRE(V2MP_Device_RelinquishConnectedMailbox(device));

					THEN("The mailbox is considered controlled by the program")
					{
						CHECK(V2MP_DevicePort_GetMailboxController(port) == V2MP_DPMC_PROGRAM);
						CHECK_FALSE(V2MP_DevicePort_IsMailboxBusy(port));
						CHECK(V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE);
						CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
						CHECK_FALSE(V2MP_DevicePort_IsMailboxFull(port));
						CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 0);
						CHECK(V2MP_DevicePort_MailboxBytesFree(port) == MAILBOX_SIZE);
						CHECK(V2MP_Device_IsConnectedMailboxEmpty(device));
						CHECK_FALSE(V2MP_Device_IsConnectedMailboxFull(device));
						CHECK(V2MP_Device_UsedBytesInConnectedMailbox(device) == 0);
						CHECK(V2MP_Device_FreeBytesInConnectedMailbox(device) == MAILBOX_SIZE);
					}

					AND_WHEN("The device relinquishes the mailbox without having control of it")
					{
						const bool secondRelinquish = V2MP_Device_RelinquishConnectedMailbox(device);

						THEN("The result of the function is false")
						{
							CHECK_FALSE(secondRelinquish);
						}
					}

					AND_WHEN("The devite attempts to write to the mailbox without having control of it")
					{
						const size_t written = V2MP_Device_WriteToConnectedMailbox(
							device,
							reinterpret_cast<const V2MP_Byte*>(MAILBOX_MESSAGE),
							sizeof(MAILBOX_MESSAGE)
						);

						THEN("No bytes are written to the mailbox")
						{
							CHECK(written == 0);
							CHECK(V2MP_DevicePort_IsMailboxEmpty(port));
							CHECK_FALSE(V2MP_DevicePort_IsMailboxFull(port));
							CHECK(V2MP_DevicePort_MailboxBytesUsed(port) == 0);
							CHECK(V2MP_DevicePort_MailboxBytesFree(port) == MAILBOX_SIZE);
							CHECK(V2MP_Device_IsConnectedMailboxEmpty(device));
							CHECK_FALSE(V2MP_Device_IsConnectedMailboxFull(device));
							CHECK(V2MP_Device_UsedBytesInConnectedMailbox(device) == 0);
							CHECK(V2MP_Device_FreeBytesInConnectedMailbox(device) == MAILBOX_SIZE);
						}
					}
				}
			}
		}

		V2MP_DevicePortCollection_DeinitAndFree(dpc);
		V2MP_DeviceCollection_DeinitAndFree(dc);
	}
}
