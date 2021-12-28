#include <vector>
#include "Helpers/TestHarnessVM.h"

TestHarnessVM::TestHarnessVM(V2MP_Word totalRamInBytes)
{
	m_Mainboard = V2MP_Mainboard_AllocateAndInit();
	m_Supervisor = V2MP_Supervisor_AllocateAndInit();

	V2MP_Supervisor_SetMainboard(m_Supervisor, m_Mainboard);
	V2MP_MemoryStore_AllocateTotalMemory(GetMemoryStore(), totalRamInBytes);

	ThrowExceptionIfNotInitialisedCorrectly(totalRamInBytes);
}

TestHarnessVM::~TestHarnessVM()
{
	V2MP_Supervisor_DeinitAndFree(m_Supervisor);
	m_Supervisor = nullptr;

	V2MP_Mainboard_DeinitAndFree(m_Mainboard);
	m_Mainboard = nullptr;
}

V2MP_Mainboard* TestHarnessVM::GetMainboard()
{
	return m_Mainboard;
}

const V2MP_Mainboard* TestHarnessVM::GetMainboard() const
{
	return m_Mainboard;
}

V2MP_Supervisor* TestHarnessVM::GetSupervisor()
{
	return m_Supervisor;
}

const V2MP_Supervisor* TestHarnessVM::GetSupervisor() const
{
	return m_Supervisor;
}

V2MP_CPU* TestHarnessVM::GetCPU()
{
	return V2MP_Mainboard_GetCPU(m_Mainboard);
}

const V2MP_CPU* TestHarnessVM::GetCPU() const
{
	return V2MP_Mainboard_GetCPU(m_Mainboard);
}

V2MP_MemoryStore* TestHarnessVM::GetMemoryStore()
{
	return V2MP_Mainboard_GetMemoryStore(m_Mainboard);
}

const V2MP_MemoryStore* TestHarnessVM::GetMemoryStore() const
{
	return V2MP_Mainboard_GetMemoryStore(m_Mainboard);
}

bool TestHarnessVM::SetCSAndDS(const V2MP_Word* cs, V2MP_Word csWords, const V2MP_Word* ds, V2MP_Word dsWords)
{
	return V2MP_Supervisor_LoadProgram(m_Supervisor, cs, csWords, ds, dsWords);
}

bool TestHarnessVM::FillCSAndDS(V2MP_Word csWords, V2MP_Word csFill, V2MP_Word dsWords, V2MP_Word dsFill)
{
	const std::vector<V2MP_Word> cs(static_cast<size_t>(csWords), csFill);
	const std::vector<V2MP_Word> ds(static_cast<size_t>(dsWords), dsFill);

	return V2MP_Supervisor_LoadProgram(m_Supervisor, cs.data(), cs.size(), ds.data(), ds.size());
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

void TestHarnessVM::ResetCPU()
{
	V2MP_CPU_Reset(GetCPU());
	OnCPUReset();
}

bool TestHarnessVM::Execute(V2MP_Word instruction)
{
	return V2MP_Supervisor_ExecuteSingleInstruction(m_Supervisor, instruction);
}

bool TestHarnessVM::GetCSWord(V2MP_Word address, V2MP_Word& outWord) const
{
	return V2MP_Supervisor_FetchCSWord(m_Supervisor, address, &outWord);
}

bool TestHarnessVM::GetDSWord(V2MP_Word address, V2MP_Word& outWord) const
{
	return V2MP_Supervisor_FetchDSWord(m_Supervisor, address, &outWord);
}

void TestHarnessVM::ThrowExceptionIfNotInitialisedCorrectly(V2MP_Word totalRamInBytes)
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

	if ( V2MP_MemoryStore_GetTotalMemorySize(GetMemoryStore()) != totalRamInBytes )
	{
		throw InitException("Memory store internal memory bank could not be allocated");
	}
}
