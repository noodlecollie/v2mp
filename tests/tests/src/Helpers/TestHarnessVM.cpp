#include <vector>
#include <cstring>
#include "Helpers/TestHarnessVM.h"
#include "SharedComponents/CircularBuffer.h"

const V2MP_Word TestHarnessVM_StartsInvalid::INVALID_WORD = 0xDEAD;

TestHarnessVM::TestHarnessVM(size_t totalRamInBytes)
{
	m_VM = V2MP_VirtualMachine_AllocateAndInit();
	V2MP_VirtualMachine_AllocateTotalMemory(m_VM, totalRamInBytes);

	ThrowExceptionIfNotInitialisedCorrectly(totalRamInBytes);
}

TestHarnessVM::~TestHarnessVM()
{
	V2MP_VirtualMachine_DeinitAndFree(m_VM);
	m_VM = nullptr;
}

V2MP_Mainboard* TestHarnessVM::GetMainboard()
{
	return V2MP_VirtualMachine_GetMainboard(m_VM);
}

const V2MP_Mainboard* TestHarnessVM::GetMainboard() const
{
	return V2MP_VirtualMachine_GetMainboard(m_VM);
}

V2MP_Supervisor* TestHarnessVM::GetSupervisor()
{
	return V2MP_VirtualMachine_GetSupervisor(m_VM);
}

const V2MP_Supervisor* TestHarnessVM::GetSupervisor() const
{
	return V2MP_VirtualMachine_GetSupervisor(m_VM);
}

V2MP_CPU* TestHarnessVM::GetCPU()
{
	return V2MP_Mainboard_GetCPU(GetMainboard());
}

const V2MP_CPU* TestHarnessVM::GetCPU() const
{
	return V2MP_Mainboard_GetCPU(GetMainboard());
}

V2MP_MemoryStore* TestHarnessVM::GetMemoryStore()
{
	return V2MP_Mainboard_GetMemoryStore(GetMainboard());
}

const V2MP_MemoryStore* TestHarnessVM::GetMemoryStore() const
{
	return V2MP_Mainboard_GetMemoryStore(GetMainboard());
}

V2MP_DevicePortCollection* TestHarnessVM::GetDevicePortCollection()
{
	return V2MP_Mainboard_GetDevicePortCollection(GetMainboard());
}

const V2MP_DevicePortCollection* TestHarnessVM::GetDevicePortCollection() const
{
	return V2MP_Mainboard_GetDevicePortCollection(GetMainboard());
}

V2MP_DeviceCollection* TestHarnessVM::GetDeviceCollection()
{
	return V2MP_Mainboard_GetDeviceCollection(GetMainboard());
}

const V2MP_DeviceCollection* TestHarnessVM::GetDeviceCollection() const
{
	return V2MP_Mainboard_GetDeviceCollection(GetMainboard());
}

bool TestHarnessVM::LoadProgram(const ProgramDef& prog)
{
	return V2MP_VirtualMachine_LoadProgram(
		m_VM,
		prog.GetCS(),
		prog.GetCSWords(),
		prog.GetDS(),
		prog.GetDSWords(),
		prog.GetSSWords()
	);
}

V2MP_DevicePort* TestHarnessVM::CreatePort(V2MP_Word address)
{
	return V2MP_DevicePortCollection_CreatePort(GetDevicePortCollection(), address);
}

bool TestHarnessVM::DestroyPort(V2MP_Word address)
{
	return V2MP_DevicePortCollection_DestroyPort(GetDevicePortCollection(), address);
}

V2MP_Word TestHarnessVM::GetCPUFaultWord() const
{
	return V2MP_CPU_GetFaultWord(GetCPU());
}

bool TestHarnessVM::CPUHasFault() const
{
	return V2MP_CPU_HasFault(GetCPU());
}

V2MP_Word TestHarnessVM::GetR0() const
{
	return V2MP_CPU_GetR0(GetCPU());
}

void TestHarnessVM::SetR0(V2MP_Word value)
{
	V2MP_CPU_SetR0(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetR1() const
{
	return V2MP_CPU_GetR1(GetCPU());
}

void TestHarnessVM::SetR1(V2MP_Word value)
{
	V2MP_CPU_SetR1(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetLR() const
{
	return V2MP_CPU_GetLinkRegister(GetCPU());
}

void TestHarnessVM::SetLR(V2MP_Word value)
{
	V2MP_CPU_SetLinkRegister(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetPC() const
{
	return V2MP_CPU_GetProgramCounter(GetCPU());
}

void TestHarnessVM::SetPC(V2MP_Word value)
{
	V2MP_CPU_SetProgramCounter(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetSR() const
{
	return V2MP_CPU_GetStatusRegister(GetCPU());
}

void TestHarnessVM::SetSR(V2MP_Word value)
{
	V2MP_CPU_SetStatusRegister(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetIR() const
{
	return V2MP_CPU_GetInstructionRegister(GetCPU());
}

V2MP_Word TestHarnessVM::GetSP() const
{
	return V2MP_CPU_GetStackPointer(GetCPU());
}

V2MP_Word TestHarnessVM::GetRegisterValue(V2MP_RegisterIndex registerIndex) const
{
	V2MP_Word out = 0;
	V2MP_CPU_GetRegisterValue(GetCPU(), registerIndex, &out);
	return out;
}

void TestHarnessVM::ResetCPU()
{
	V2MP_CPU_Reset(GetCPU());
	OnCPUReset();
}

bool TestHarnessVM::Execute(V2MP_Word instruction)
{
	return V2MP_Supervisor_ExecuteSingleInstruction(GetSupervisor(), instruction);
}

bool TestHarnessVM::GetCSWord(V2MP_Word address, V2MP_Word& outWord) const
{
	return V2MP_Supervisor_FetchCSWord(GetSupervisor(), address, &outWord);
}

bool TestHarnessVM::GetDSWord(V2MP_Word address, V2MP_Word& outWord) const
{
	return V2MP_Supervisor_FetchDSWord(GetSupervisor(), address, &outWord);
}

bool TestHarnessVM::GetDSData(V2MP_Word address, size_t length, std::vector<V2MP_Byte>& outData)
{
	outData.clear();
	outData.resize(length);

	return V2MP_Supervisor_ReadDSRange(GetSupervisor(), address, outData.data(), outData.size());
}

bool TestHarnessVM::GetSSData(V2MP_Word address, size_t lengthInWords, std::vector<V2MP_Word>& outWords)
{
	outWords.clear();
	outWords.resize(lengthInWords);

	if ( address & 0x1 )
	{
		return false;
	}

	return V2MP_Supervisor_ReadSSRange(
		GetSupervisor(),
		address,
		reinterpret_cast<V2MP_Byte*>(outWords.data()),
		outWords.size() * sizeof(V2MP_Word)
	);
}

std::shared_ptr<BaseMockDevice> TestHarnessVM::GetBaseMockDevice(V2MP_Word address) const
{
	const auto it = m_MockDevices.find(address);
	return it != m_MockDevices.end() ? it->second : std::shared_ptr<BaseMockDevice>();
}

void TestHarnessVM::ThrowExceptionIfNotInitialisedCorrectly(size_t totalRamInBytes)
{
	if ( !GetMainboard() )
	{
		throw InitException("Mainboard could not be allocated");
	}

	if ( !GetSupervisor() )
	{
		throw InitException("Supervisor could not be allocated");
	}

	if ( !GetCPU() )
	{
		throw InitException("CPU could not be allocated");
	}

	if ( !GetMemoryStore() )
	{
		throw InitException("Memory store could not be allocated");
	}

	if ( V2MP_VirtualMachine_GetTotalMemoryBytes(m_VM) != totalRamInBytes )
	{
		throw InitException("Memory store internal memory bank could not be allocated");
	}
}

bool TestHarnessVM::ConnectMockDeviceToPortInternal(std::shared_ptr<BaseMockDevice> mockDevice, V2MP_Word address)
{
	if ( !mockDevice || m_MockDevices.find(address) != m_MockDevices.end() )
	{
		return false;
	}

	V2MP_DevicePort* port = nullptr;
	V2MP_Device* device = nullptr;
	bool createdPort = false;

	do
	{
		V2MP_DevicePortCollection* dpc = GetDevicePortCollection();

		if ( !dpc )
		{
			break;
		}

		V2MP_DeviceCollection* dc = GetDeviceCollection();

		if ( !dc )
		{
			break;
		}

		port = V2MP_DevicePortCollection_GetPort(dpc, address);

		if ( !port )
		{
			port = V2MP_DevicePortCollection_CreatePort(dpc, address);
			createdPort = true;

			if ( !port )
			{
				break;
			}
		}

		if ( V2MP_DevicePort_HasConnectedDevice(port) )
		{
			break;
		}

		device = V2MP_DeviceCollection_CreateDevice(dc);

		if ( !device || !V2MP_DevicePort_ConnectDevice(port, device) )
		{
			break;
		}

		mockDevice->AttachToV2MPDevice(device);
		m_MockDevices[address] = mockDevice;

		return true;
	}
	while ( false );

	if ( device )
	{
		V2MP_DeviceCollection_DestroyDevice(GetDeviceCollection(), device);
	}

	if ( port && createdPort )
	{
		V2MP_DevicePortCollection_DestroyPort(GetDevicePortCollection(), address);
	}

	return false;
}
