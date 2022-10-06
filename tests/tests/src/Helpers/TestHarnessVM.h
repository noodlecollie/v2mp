#pragma once

#include <stdexcept>
#include <vector>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/VirtualMachine.h"
#include "V2MPInternal/Modules/Mainboard.h"
#include "V2MPInternal/Modules/Supervisor.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"

class TestHarnessVM
{
public:
	static constexpr size_t MAX_WORD_VALUE_AS_SIZE_T = static_cast<std::size_t>(static_cast<V2MP_Word>(~0));
	static constexpr size_t DEFAULT_RAM_BYTES = 64;

	class ProgramDef
	{
	public:
		inline void SetCSAndDS(const V2MP_Word* csData, V2MP_Word numCSWords, const V2MP_Word* dsData, V2MP_Word numDSWords)
		{
			m_ShouldFill = false;
			m_CS = csData;
			m_CSWords = numCSWords;
			m_DS = dsData;
			m_DSWords = numDSWords;
		}

		inline void SetCS(const V2MP_Word* csData, V2MP_Word numCSWords)
		{
			SetCSAndDS(csData, numCSWords, nullptr, 0);
		}

		template<std::size_t CSN, size_t DSN>
		inline typename std::enable_if<CSN <= MAX_WORD_VALUE_AS_SIZE_T && DSN <= MAX_WORD_VALUE_AS_SIZE_T, void>::type
		SetCSAndDS(const V2MP_Word (&csData)[CSN], const V2MP_Word (&dsData)[DSN])
		{
			SetCSAndDS(csData, static_cast<V2MP_Word>(CSN), dsData, static_cast<V2MP_Word>(DSN));
		}

		template<std::size_t CSN>
		inline typename std::enable_if<CSN <= MAX_WORD_VALUE_AS_SIZE_T, void>::type
		SetCS(const V2MP_Word (&csData)[CSN])
		{
			SetCSAndDS(csData, static_cast<V2MP_Word>(CSN), nullptr, 0);
		}

		inline void FillCSAndDS(V2MP_Word numCSWords, V2MP_Word csFill, V2MP_Word numDSWords, V2MP_Word dsFill)
		{
			m_ShouldFill = true;

			m_CSFill.clear();
			m_CSFill.resize(static_cast<size_t>(numCSWords), csFill);

			m_DSFill.clear();
			m_DSFill.resize(static_cast<size_t>(numDSWords), dsFill);

			m_CS = m_CSFill.data();
			m_CSWords = m_CSFill.size();
			m_DS = m_DSFill.data();
			m_DSWords = m_DSFill.size();
		}

		inline void FillCS(V2MP_Word numCSWords, V2MP_Word csFill)
		{
			FillCSAndDS(numCSWords, csFill, 0, 0);
		}

		inline void SetCSAndDS(const std::vector<V2MP_Word>& cs, const std::vector<V2MP_Word>& ds)
		{
			if ( cs.size() > MAX_WORD_VALUE_AS_SIZE_T || ds.size() > MAX_WORD_VALUE_AS_SIZE_T )
			{
				return;
			}

			SetCSAndDS(cs.data(), static_cast<V2MP_Word>(cs.size()), ds.data(), static_cast<V2MP_Word>(ds.size()));
		}

		inline void SetCS(const std::vector<V2MP_Word>& cs)
		{
			SetCSAndDS(cs, std::vector<V2MP_Word>());
		}

		inline void SetStackSize(V2MP_Word numWords)
		{
			m_SSWords = numWords;
		}

		inline bool GetShouldFill() const
		{
			return m_ShouldFill;
		}

		inline const V2MP_Word* GetCS() const
		{
			return m_ShouldFill ? m_CSFill.data() : m_CS;
		}

		inline size_t GetCSWords() const
		{
			return m_CSWords;
		}

		inline const V2MP_Word* GetDS() const
		{
			return m_ShouldFill ? m_DSFill.data() : m_DS;
		}

		inline size_t GetDSWords() const
		{
			return m_DSWords;
		}

		inline size_t GetSSWords() const
		{
			return m_SSWords;
		}

	private:
		const V2MP_Word* m_CS = nullptr;
		size_t m_CSWords = 0;
		const V2MP_Word* m_DS = nullptr;
		size_t m_DSWords = 0;
		size_t m_SSWords = 0;

		bool m_ShouldFill = false;
		std::vector<V2MP_Word> m_CSFill;
		std::vector<V2MP_Word> m_DSFill;
	};

	class InitException : public std::runtime_error
	{
	public:
		inline InitException(const std::string& description) :
			std::runtime_error("Failed to initialise VM: " + description)
		{
		}
	};

	TestHarnessVM(size_t totalRamInBytes = DEFAULT_RAM_BYTES);
	virtual ~TestHarnessVM();

	TestHarnessVM(const TestHarnessVM& other) = delete;
	TestHarnessVM(TestHarnessVM&& other) = delete;
	TestHarnessVM& operator =(const TestHarnessVM& other) = delete;
	TestHarnessVM& operator =(TestHarnessVM&& other) = delete;

	V2MP_Mainboard* GetMainboard();
	const V2MP_Mainboard* GetMainboard() const;

	V2MP_Supervisor* GetSupervisor();
	const V2MP_Supervisor* GetSupervisor() const;

	V2MP_CPU* GetCPU();
	const V2MP_CPU* GetCPU() const;

	V2MP_MemoryStore* GetMemoryStore();
	const V2MP_MemoryStore* GetMemoryStore() const;

	bool LoadProgram(const ProgramDef& prog);

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
	V2MP_Word GetSP() const;

	V2MP_Word GetRegisterValue(V2MP_RegisterIndex registerIndex) const;

	void ResetCPU();
	bool Execute(V2MP_Word instruction);

	bool GetCSWord(V2MP_Word address, V2MP_Word& outWord) const;
	bool GetDSWord(V2MP_Word address, V2MP_Word& outWord) const;

	bool GetDSData(V2MP_Word address, size_t length, std::vector<V2MP_Byte>& outData);
	bool GetSSData(V2MP_Word address, size_t lengthInWords, std::vector<V2MP_Word>& outWords);

protected:
	virtual void OnCPUReset()
	{
	}

private:
	void ThrowExceptionIfNotInitialisedCorrectly(size_t totalRamInBytes);

	V2MP_VirtualMachine* m_VM;
};

class TestHarnessVM_StartsInvalid : public TestHarnessVM
{
public:
	// For some weird reason this can't be defined inline,
	// even it it's constexpr... Dammit, GCC.
	static const V2MP_Word INVALID_WORD;

	inline TestHarnessVM_StartsInvalid(size_t totalRamInBytes = TestHarnessVM::DEFAULT_RAM_BYTES) :
		TestHarnessVM(totalRamInBytes)
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
