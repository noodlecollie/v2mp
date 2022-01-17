#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"
#include "Helpers/EmitterMockDevice.h"

SCENARIO("DPO: Getting the usable byte count of a port's mailbox returns the correct number of bytes", "[instructions]")
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
		prog.FillCSAndDS(SEGMENT_SIZE_WORDS, 0, SEGMENT_SIZE_WORDS, 0);

		REQUIRE(vm.LoadProgram(prog));

		AND_GIVEN("A device port with no device connected")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);
			REQUIRE_FALSE(V2MP_DevicePort_HasConnectedDevice(port));

			WHEN("A DPO instruction is executed to get the usable byte count")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::USABLE_BYTE_COUNT)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The count is zero")
				{
					CHECK(vm.GetR0() == PORT_ADDRESS);
					CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				}
			}
		}

		AND_GIVEN("A device port with no mailbox allocated")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);
			REQUIRE(V2MP_DevicePort_HasConnectedDevice(port));

			WHEN("A DPO instruction is executed to get the usable byte count")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::USABLE_BYTE_COUNT)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The count is zero")
				{
					CHECK(vm.GetR0() == PORT_ADDRESS);
					CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetLR() == 0);
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				}
			}
		}

		AND_GIVEN("A readable port with a message in its mailbox")
		{
			static constexpr const char MESSAGE[] = "Mailbox message";

			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(device->WriteToConnectedMailbox(MESSAGE) == sizeof(MESSAGE));
			REQUIRE(V2MP_DevicePort_MailboxBytesUsed(port) == sizeof(MESSAGE));
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to get the usable byte count")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::USABLE_BYTE_COUNT)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The count is equal to the used space in the mailbox")
				{
					CHECK(vm.GetR0() == PORT_ADDRESS);
					CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(V2MP_DevicePort_MailboxBytesUsed(port)));
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				}
			}
		}

		AND_GIVEN("A writeable port with no message in its mailbox")
		{
			V2MP_DevicePort* port = vm.CreatePort(PORT_ADDRESS);
			REQUIRE(port);

			std::shared_ptr<EmitterMockDevice> device = vm.ConnectMockDeviceToPort<EmitterMockDevice>(PORT_ADDRESS);
			REQUIRE(device);

			REQUIRE(device->AllocateConnectedMailbox(MAILBOX_SIZE));
			REQUIRE(V2MP_DevicePort_MailboxBytesFree(port) == MAILBOX_SIZE);
			REQUIRE(device->RelinquishConnectedMailbox());

			WHEN("A DPO instruction is executed to get the usable byte count")
			{
				vm.SetR0(PORT_ADDRESS);

				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::USABLE_BYTE_COUNT)));
				REQUIRE_FALSE(vm.CPUHasFault());

				THEN("The count is equal to the free space in the mailbox")
				{
					CHECK(vm.GetR0() == PORT_ADDRESS);
					CHECK(vm.GetR1() == TestHarnessVM_StartsInvalid::INVALID_WORD);
					CHECK(vm.GetLR() == static_cast<V2MP_Word>(V2MP_DevicePort_MailboxBytesFree(port)));
					CHECK(vm.GetPC() == TestHarnessVM_StartsInvalid::INVALID_WORD);
				}
			}
		}
	}
}

SCENARIO("DPO: Setting any reserved bit raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0001;
		static constexpr V2MP_Word VAL_R1 = 0x0002;
		static constexpr V2MP_Word VAL_LR = 0x0003;
		static constexpr V2MP_Word VAL_PC = 0x0004;

		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		for ( size_t index = 2; index <= 10; ++index )
		{
			WHEN("A DPO instruction is executed with a reserved bit set")
			{
				REQUIRE(vm.Execute(Asm::DPO(Asm::DevicePortOperation::WRITE, true) | (1 << index)));

				THEN("A RES fault is raised, and all registers are left unchanged")
				{
					INFO("Reserved bit " << index << " was set");
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK(vm.GetSR() == 0);
				}
			}
		}
	}
}

// TODO: IDT instruction bit set on usable byte count or relinquish instruction
