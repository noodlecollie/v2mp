#pragma once

#include <cstddef>
#include "V2MP/Defs.h"
#include "V2MP/CPU.h"
#include "V2MP/MemoryStore.h"

class MinimalVirtualMachine
{
public:
	MinimalVirtualMachine();
	virtual ~MinimalVirtualMachine();

	MinimalVirtualMachine(const MinimalVirtualMachine& other) = delete;
	MinimalVirtualMachine(MinimalVirtualMachine&& other) = delete;
	MinimalVirtualMachine& operator =(const MinimalVirtualMachine& other) = delete;
	MinimalVirtualMachine& operator =(MinimalVirtualMachine&& other) = delete;

	template<std::size_t N>
	bool SetCS(const V2MP_Word (&data)[N])
	{
		if ( N > static_cast<std::size_t>(static_cast<V2MP_Word>(~0)) )
		{
			return false;
		}

		return V2MP_MemoryStore_AllocateCS(&m_MemoryStore, static_cast<V2MP_Word>(N), data);
	}

	template<std::size_t N>
	bool SetDS(const V2MP_Word (&data)[N])
	{
		if ( N > static_cast<std::size_t>(static_cast<V2MP_Word>(~0)) )
		{
			return false;
		}

		return V2MP_MemoryStore_AllocateDS(&m_MemoryStore, static_cast<V2MP_Word>(N), data);
	}

	bool SetCS(const V2MP_Word* data, V2MP_Word numWords);
	bool SetDS(const V2MP_Word* data, V2MP_Word numWords);

	bool AllocateCS(V2MP_Word numWords, V2MP_Word fill = 0x0000);
	bool AllocateDS(V2MP_Word numWords, V2MP_Word fill = 0x0000);

	const V2MP_MemoryStore* GetMemoryStore() const;
	V2MP_MemoryStore* GetMemoryStore();

	const V2MP_CPU* GetCPU() const;
	V2MP_CPU* GetCPU();

	bool FetchDecodeExecute();
	bool Execute(V2MP_Word instruction);

	V2MP_Word GetCPUFault() const;
	bool CPUHasFault() const;

	V2MP_Word GetR0() const;
	void SetR0(V2MP_Word value);

	V2MP_Word GetR1() const;
	void SetR1(V2MP_Word value);

	V2MP_Word GetLR() const;
	void SetLR(V2MP_Word value);

	V2MP_Word GetPC() const;
	void SetPC(V2MP_Word value);

	V2MP_Word GetSR() const;

	bool GetCSWord(V2MP_Word address, V2MP_Word& outWord, V2MP_Fault* outFault = nullptr) const;
	bool GetDSWord(V2MP_Word address, V2MP_Word& outWord, V2MP_Fault* outFault = nullptr) const;

private:
	V2MP_MemoryStore m_MemoryStore;
	V2MP_CPU m_CPU;
};
