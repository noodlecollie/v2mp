#include <cstdlib>
#include <vector>
#include "Helpers/MinimalVirtualMachine.h"

const V2MP_Word VM_StartsInvalid::INVALID_WORD = 0xDEAD;

MinimalVirtualMachine::MinimalVirtualMachine()
{
	m_MemoryStore = V2MP_MemoryStore_AllocateAndInit();

	m_CPU = V2MP_CPU_AllocateAndInit();
	V2MP_CPU_SetMemoryStore(m_CPU, m_MemoryStore);
}

MinimalVirtualMachine::~MinimalVirtualMachine()
{
	V2MP_CPU_DeinitAndFree(m_CPU);
	m_CPU = NULL;

	V2MP_MemoryStore_DeinitAndFree(m_MemoryStore);
	m_MemoryStore = NULL;
}

bool MinimalVirtualMachine::SetCS(const V2MP_Word* data, V2MP_Word numWords)
{
	return V2MP_MemoryStore_AllocateCS(m_MemoryStore, numWords, data);
}

bool MinimalVirtualMachine::SetDS(const V2MP_Word* data, V2MP_Word numWords)
{
	return V2MP_MemoryStore_AllocateDS(m_MemoryStore, numWords, data);
}

bool MinimalVirtualMachine::AllocateCS(V2MP_Word numWords, V2MP_Word fill)
{
	std::vector<V2MP_Word> data(static_cast<size_t>(numWords), fill);
	return V2MP_MemoryStore_AllocateCS(m_MemoryStore, numWords, data.data());
}

bool MinimalVirtualMachine::AllocateDS(V2MP_Word numWords, V2MP_Word fill)
{
	std::vector<V2MP_Word> data(static_cast<size_t>(numWords), fill);
	return V2MP_MemoryStore_AllocateDS(m_MemoryStore, numWords, data.data());
}

const V2MP_MemoryStore* MinimalVirtualMachine::GetMemoryStore() const
{
	return m_MemoryStore;
}

V2MP_MemoryStore* MinimalVirtualMachine::GetMemoryStore()
{
	return m_MemoryStore;
}

const V2MP_CPU* MinimalVirtualMachine::GetCPU() const
{
	return m_CPU;
}

V2MP_CPU* MinimalVirtualMachine::GetCPU()
{
	return m_CPU;
}

void MinimalVirtualMachine::ResetCPU()
{
	V2MP_CPU_Reset(m_CPU);
	OnCPUReset();
}

bool MinimalVirtualMachine::FetchDecodeExecute()
{
	return V2MP_CPU_FetchDecodeAndExecuteInstruction(m_CPU);
}

bool MinimalVirtualMachine::Execute(V2MP_Word instruction)
{
	return V2MP_CPU_ExecuteInstruction(m_CPU, instruction);
}

V2MP_Word MinimalVirtualMachine::GetCPUFault() const
{
	return V2MP_CPU_GetFault(m_CPU);
}

bool MinimalVirtualMachine::CPUHasFault() const
{
	const V2MP_Word fault = GetCPUFault();
	return V2MP_CPU_FAULT_CODE(fault) != V2MP_FAULT_NONE;
}

V2MP_Word MinimalVirtualMachine::GetR0() const
{
	return V2MP_CPU_GetR0(m_CPU);
}

void MinimalVirtualMachine::SetR0(V2MP_Word value)
{
	V2MP_CPU_SetR0(m_CPU, value);
}

V2MP_Word MinimalVirtualMachine::GetR1() const
{
	return V2MP_CPU_GetR1(m_CPU);
}

void MinimalVirtualMachine::SetR1(V2MP_Word value)
{
	V2MP_CPU_SetR1(m_CPU, value);
}

V2MP_Word MinimalVirtualMachine::GetLR() const
{
	return V2MP_CPU_GetLinkRegister(m_CPU);
}

void MinimalVirtualMachine::SetLR(V2MP_Word value)
{
	V2MP_CPU_SetLinkRegister(m_CPU, value);
}

V2MP_Word MinimalVirtualMachine::GetPC() const
{
	return V2MP_CPU_GetProgramCounter(m_CPU);
}

void MinimalVirtualMachine::SetPC(V2MP_Word value)
{
	V2MP_CPU_SetProgramCounter(m_CPU, value);
}

V2MP_Word MinimalVirtualMachine::GetSR() const
{
	return V2MP_CPU_GetStatusRegister(m_CPU);
}

void MinimalVirtualMachine::SetSR(V2MP_Word value)
{
	V2MP_CPU_SetStatusRegister(m_CPU, value);
}

V2MP_Word MinimalVirtualMachine::GetIR() const
{
	return V2MP_CPU_GetInstructionRegister(m_CPU);
}

bool MinimalVirtualMachine::GetCSWord(V2MP_Word address, V2MP_Word& outWord, V2MP_Fault* outFault) const
{
	return V2MP_MemoryStore_FetchCSWord(m_MemoryStore, address, &outWord, outFault);
}

bool MinimalVirtualMachine::GetDSWord(V2MP_Word address, V2MP_Word& outWord, V2MP_Fault* outFault) const
{
	return V2MP_MemoryStore_FetchDSWord(m_MemoryStore, address, &outWord, outFault);
}
