#include <vector>
#include "Helpers/TestHarnessVM.h"

TestHarnessVM::TestHarnessVM()
{
	m_Mainboard = V2MP_Mainboard_AllocateAndInit();
	m_Supervisor = V2MP_Supervisor_AllocateAndInit();

	V2MP_Supervisor_SetMainboard(m_Supervisor, m_Mainboard);

	ThrowExceptionIfNotInitialisedCorrectly();
}

TestHarnessVM::~TestHarnessVM()
{
	V2MP_Supervisor_DeinitAndFree(m_Supervisor);
	V2MP_Mainboard_DeinitAndFree(m_Mainboard);
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

V2MP_CPURenameMe* TestHarnessVM::GetCPU()
{
	return V2MP_Mainboard_GetCPU(m_Mainboard);
}

const V2MP_CPURenameMe* TestHarnessVM::GetCPU() const
{
	return V2MP_Mainboard_GetCPU(m_Mainboard);
}

V2MP_MemoryStoreRenameMe* TestHarnessVM::GetMemoryStore()
{
	return V2MP_Mainboard_GetMemoryStore(m_Mainboard);
}

const V2MP_MemoryStoreRenameMe* TestHarnessVM::GetMemoryStore() const
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
	return V2MP_CPURenameMe_GetFaultWord(GetCPU());
}

bool TestHarnessVM::CPUHasFault() const
{
	return V2MP_CPURenameMe_HasFault(GetCPU());
}

V2MP_Word TestHarnessVM::GetR0() const
{
	return V2MP_CPURenameMe_GetR0(GetCPU());
}

void TestHarnessVM::SetR0(V2MP_Word value)
{
	V2MP_CPURenameMe_SetR0(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetR1() const
{
	return V2MP_CPURenameMe_GetR1(GetCPU());
}

void TestHarnessVM::SetR1(V2MP_Word value)
{
	V2MP_CPURenameMe_SetR1(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetLR() const
{
	return V2MP_CPURenameMe_GetLinkRegister(GetCPU());
}

void TestHarnessVM::SetLR(V2MP_Word value)
{
	V2MP_CPURenameMe_SetLinkRegister(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetPC() const
{
	return V2MP_CPURenameMe_GetProgramCounter(GetCPU());
}

void TestHarnessVM::SetPC(V2MP_Word value)
{
	V2MP_CPURenameMe_SetProgramCounter(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetSR() const
{
	return V2MP_CPURenameMe_GetStatusRegister(GetCPU());
}

void TestHarnessVM::SetSR(V2MP_Word value)
{
	V2MP_CPURenameMe_SetStatusRegister(GetCPU(), value);
}

V2MP_Word TestHarnessVM::GetIR() const
{
	return V2MP_CPURenameMe_GetInstructionRegister(GetCPU());
}

bool TestHarnessVM::GetCSWord(V2MP_Word address, V2MP_Word& outWord) const
{
	// TODO: We need to add functions to the supervisor for loading words from CS/DS.
	(void)address;
	(void)outWord;
	return false;
}

bool TestHarnessVM::GetDSWord(V2MP_Word address, V2MP_Word& outWord) const
{
	// TODO: We need to add functions to the supervisor for loading words from CS/DS.
	(void)address;
	(void)outWord;
	return false;
}

void TestHarnessVM::ThrowExceptionIfNotInitialisedCorrectly()
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
}
