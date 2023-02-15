#include "ProgramModel/ValidationUtils.h"
#include "Utils/StringUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	ValidationFailure CreateTooManyArgumentsFailure(size_t expected, size_t actual, size_t argIndex)
	{
		return ValidationFailure(
			PublicWarningID::TOO_MANY_ARGUMENTS,
			argIndex,
			"Expected " + std::to_string(expected) +
			" arguments to instruction but got " + std::to_string(actual) +
			" arguments. Additional arguments ignored."
		);
	}

	ValidationFailure CreateTooFewArgumentsFailure(size_t expected, size_t actual, size_t argIndex)
	{
		return ValidationFailure(
			PublicErrorID::TOO_FEW_ARGUMENTS,
			argIndex,
			"Expected " + std::to_string(expected) +
			" arguments to instruction but got " + std::to_string(actual) + " arguments."
		);
	}

	ValidationFailure CreateArgumentOutOfRangeFailure(
		int32_t minValue,
		int32_t maxValue,
		int32_t origValue,
		int32_t newValue,
		size_t argIndex)
	{
		return ValidationFailure(
			PublicWarningID::ARG_OUT_OF_RANGE,
			argIndex,
			"Value " + DecAndHexString(origValue) + " was out of valid range " +
			DecAndHexString(minValue) + " - " + DecAndHexString(maxValue) + ", truncating to " +
			DecAndHexString(newValue) + "."
		);
	}

	ValidationFailure CreateLabelRefValueOutOfRangeFailure(
		const std::string& labelName,
		int32_t minValue,
		int32_t maxValue,
		int32_t origValue,
		size_t argIndex)
	{
		return ValidationFailure(
			PublicErrorID::LABEL_REF_OUT_OF_RANGE,
			argIndex,
			"Value " + DecAndHexString(origValue) + " for reference to label \"" + labelName +
			"\" was out of valid range " +
			DecAndHexString(minValue) + " - " + DecAndHexString(maxValue) + "."
		);
	}

	ValidationFailure CreateLabelRefUsedForReservedBitsFailure(
		size_t argIndex)
	{
		return ValidationFailure(
			PublicErrorID::INVALID_ARGUMENT_TYPE,
			argIndex,
			"This argument is reserved in this context, and so must be set to zero. "
			"A label reference may not be used here."
		);
	}

	ValidationFailure CreateReservedBitsSetFailure(
		size_t argIndex)
	{
		return ValidationFailure(
			PublicWarningID::RESERVED_BITS_SET,
			argIndex,
			"This argument is reserved in this context. Its value will be set to zero."
		);
	}

	AssemblerException ToAssemblerException(
		const ValidationFailure& failure,
		const std::string& filePath,
		const CodeWord& codeWord
	)
	{
		const size_t column = failure.GetArgIndex() < codeWord.GetArgumentCount()
			? codeWord.GetArgumentColumn(failure.GetArgIndex())
			: codeWord.GetColumn();

		if ( failure.IsError() )
		{
			return AssemblerException(
				failure.GetErrorID(),
				filePath,
				codeWord.GetLine(),
				column,
				failure.GetMessage()
			);
		}
		else
		{
			return AssemblerException(
				failure.GetWarningID(),
				filePath,
				codeWord.GetLine(),
				column,
				failure.GetMessage()
			);
		}
	}
}
