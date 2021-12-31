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
#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Modules/DeviceCollection.h"
#include "V2MPInternal/Modules/Device.h"
#include "Helpers/MockDevice.h"

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

	V2MP_DevicePortCollection* GetDevicePortCollection();
	const V2MP_DevicePortCollection* GetDevicePortCollection() const;

	V2MP_DeviceCollection* GetDeviceCollection();
	const V2MP_DeviceCollection* GetDeviceCollection() const;

	bool SetCSAndDS(const V2MP_Word* cs, size_t csWords, const V2MP_Word* ds, size_t dsWords);
	bool FillCSAndDS(size_t csWords, V2MP_Word csFill, size_t dsWords, V2MP_Word dsFill);

	inline bool SetCS(const V2MP_Word* cs, size_t csWords)
	{
		return SetCSAndDS(cs, csWords, nullptr, 0);
	}

	inline bool FillCS(size_t csWords, V2MP_Word csFill)
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

	V2MP_DevicePort* CreatePort(V2MP_Word address, size_t mailboxSize);
	bool DestroyPort(V2MP_Word address);
	size_t WriteToPortMailbox(V2MP_Word address, const char* string);

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

	bool GetDSData(V2MP_Word address, size_t length, std::vector<V2MP_Byte>& outData);

	template<typename T>
	inline typename std::enable_if<std::is_base_of<MockDevice, T>::value, std::shared_ptr<T>>::type
	ConnectMockDeviceToPort(V2MP_Word address)
	{
		std::shared_ptr<T> mockDeviceSubclass = std::make_shared<T>();

		return ConnectMockDeviceToPortInternal(std::static_pointer_cast<MockDevice>(mockDeviceSubclass), address)
			? mockDeviceSubclass
			: std::shared_ptr<T>();
	}

protected:
	virtual void OnCPUReset()
	{
	}

private:
	void ThrowExceptionIfNotInitialisedCorrectly(size_t totalRamInBytes);
	bool ConnectMockDeviceToPortInternal(std::shared_ptr<MockDevice> mockDevice, V2MP_Word address);

	V2MP_VirtualMachine* m_VM;
	std::unordered_map<V2MP_Word, std::shared_ptr<MockDevice>> m_MockDevices;
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
