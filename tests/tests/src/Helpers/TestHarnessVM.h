#pragma once

#include <stdexcept>
#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/Mainboard.h"
#include "V2MPInternal/Modules/Supervisor.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"

class TestHarnessVM
{
public:
	static constexpr size_t MAX_WORD_VALUE_AS_SIZE_T = static_cast<std::size_t>(static_cast<V2MP_Word>(~0));
	static constexpr size_t DEFAULT_RAM_BYTES = 64;

	class InitException : public std::runtime_error
	{
	public:
		inline InitException(const std::string& description) :
			std::runtime_error("Failed to initialise VM: " + description)
		{
		}
	};

	TestHarnessVM(V2MP_Word totalRamInBytes = DEFAULT_RAM_BYTES);
	virtual ~TestHarnessVM();

	TestHarnessVM(const TestHarnessVM& other) = delete;
	TestHarnessVM(TestHarnessVM&& other) = delete;
	TestHarnessVM& operator =(const TestHarnessVM& other) = delete;
	TestHarnessVM& operator =(TestHarnessVM&& other) = delete;

	V2MP_Mainboard* GetMainboard();
	const V2MP_Mainboard* GetMainboard() const;

	V2MP_Supervisor* GetSupervisor();
	const V2MP_Supervisor* GetSupervisor() const;

	V2MP_CPURenameMe* GetCPU();
	const V2MP_CPURenameMe* GetCPU() const;

	V2MP_MemoryStoreRenameMe* GetMemoryStore();
	const V2MP_MemoryStoreRenameMe* GetMemoryStore() const;

	bool SetCSAndDS(const V2MP_Word* cs, V2MP_Word csWords, const V2MP_Word* ds, V2MP_Word dsWords);
	bool FillCSAndDS(V2MP_Word csWords, V2MP_Word csFill, V2MP_Word dsWords, V2MP_Word dsFill);

	inline bool SetCS(const V2MP_Word* cs, V2MP_Word csWords)
	{
		return SetCSAndDS(cs, csWords, nullptr, 0);
	}

	inline bool FillCS(V2MP_Word csWords, V2MP_Word csFill)
	{
		return FillCSAndDS(csWords, csFill, 0, 0);
	}

	template<std::size_t CSN, std::size_t DSN>
	inline typename std::enable_if<CSN <= MAX_WORD_VALUE_AS_SIZE_T && DSN <= MAX_WORD_VALUE_AS_SIZE_T, bool>::type
	SetCSAndDS(const V2MP_Word (&csData)[CSN], const V2MP_Word (&dsData)[DSN])
	{
		return SetCSAndDS(csData, CSN, dsData, DSN);
	}

	template<std::size_t CSN>
	inline typename std::enable_if<CSN <= MAX_WORD_VALUE_AS_SIZE_T, bool>::type
	SetCS(const V2MP_Word (&csData)[CSN])
	{
		return SetCSAndDS(csData, CSN, nullptr, 0);
	}

	V2MP_Word GetCPUFaultWord() const;
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
	void SetSR(V2MP_Word value);

	V2MP_Word GetIR() const;

	void ResetCPU();
	bool Execute(V2MP_Word instruction);

	bool GetCSWord(V2MP_Word address, V2MP_Word& outWord) const;
	bool GetDSWord(V2MP_Word address, V2MP_Word& outWord) const;

protected:
	virtual void OnCPUReset()
	{
	}

private:
	void ThrowExceptionIfNotInitialisedCorrectly(V2MP_Word totalRamInBytes);

	V2MP_Mainboard* m_Mainboard = nullptr;
	V2MP_Supervisor* m_Supervisor = nullptr;
};

class TestHarnessVM_StartsInvalid : public TestHarnessVM
{
public:
	static constexpr V2MP_Word INVALID_WORD = 0xDEAD;

	inline TestHarnessVM_StartsInvalid() :
		TestHarnessVM()
	{
		SetUpRegisters();
	}

	virtual ~TestHarnessVM_StartsInvalid() = default;

protected:
	void OnCPUReset() override
	{
		SetUpRegisters();
	}

private:
	void SetUpRegisters()
	{
		SetR0(INVALID_WORD);
		SetR1(INVALID_WORD);
		SetLR(INVALID_WORD);
		SetPC(INVALID_WORD);
	}
};
