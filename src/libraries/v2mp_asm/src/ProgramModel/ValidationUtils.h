#pragma once

#include <string>
#include <cstdint>
#include "ProgramModel/ValidationFailure.h"
#include "Exceptions/AssemblerException.h"

namespace V2MPAsm
{
	class CodeWord;

	static constexpr int32_t MaxUnsignedValue(size_t numBits)
	{
		// (2 ^ n) - 1;
		return (1 << numBits) - 1;
	}

	static constexpr int32_t MinSignedValue(size_t numBits)
	{
		// -(2 ^ (n-1))
		return numBits > 1 ? (static_cast<int32_t>(1 << (numBits - 1)) * -1) : -1;
	}

	static constexpr int32_t MaxSignedValue(size_t numBits)
	{
		// (2 ^ (n-1)) - 1
		return numBits > 1 ? (1 << (numBits - 1)) : 0;
	}

	static constexpr uint32_t BitMask(size_t numBits)
	{
		return static_cast<uint32_t>(~0) >> ((sizeof(uint32_t) * 8) - numBits);
	}

	ValidationFailure CreateTooManyArgumentsFailure(size_t expected, size_t actual, size_t argIndex = 0);
	ValidationFailure CreateTooFewArgumentsFailure(size_t expected, size_t actual, size_t argIndex = 0);

	ValidationFailure CreateArgumentOutOfRangeFailure(
		int32_t minValue,
		int32_t maxValue,
		int32_t origValue,
		int32_t newValue,
		size_t argIndex = 0);

	ValidationFailure CreateLabelRefValueOutOfRangeFailure(
		const std::string& labelName,
		int32_t minValue,
		int32_t maxValue,
		int32_t origValue,
		size_t argIndex = 0);

	ValidationFailure CreateLabelRefUsedForReservedBitsFailure(size_t argIndex = 0);
	ValidationFailure CreateReservedBitsSetFailure(size_t argIndex = 0);

	AssemblerException ToAssemblerException(
		const ValidationFailure& failure,
		const std::string& filePath,
		const CodeWord& codeWord
	);
}
