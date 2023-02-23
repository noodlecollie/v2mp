#pragma once

#include <string>
#include <cstdint>
#include "ProgramModel/ValidationFailure.h"
#include "Exceptions/AssemblerException.h"

namespace V2MPAsm
{
	class CodeWord;

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
