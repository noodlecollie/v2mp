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

	class InitException : public std::runtime_error
	{
	public:
		inline InitException(const std::string& description) :
			std::runtime_error("Failed to initialise VM: " + description)
		{
		}
	};

	TestHarnessVM();
	~TestHarnessVM();

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

	bool GetCSWord(V2MP_Word address, V2MP_Word& outWord) const;
	bool GetDSWord(V2MP_Word address, V2MP_Word& outWord) const;

private:
	void ThrowExceptionIfNotInitialisedCorrectly();

	V2MP_Mainboard* m_Mainboard = nullptr;
	V2MP_Supervisor* m_Supervisor = nullptr;
};
