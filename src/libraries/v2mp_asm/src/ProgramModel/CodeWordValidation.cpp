#include <cassert>
#include "ProgramModel/CodeWordValidation.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/InstructionMeta.h"
#include "ProgramModel/ProgramModel.h"
#include "Utils/StringUtils.h"

namespace V2MPAsm
{
	ValidationFailure::ValidationFailure(
		PublicWarningID warning,
		size_t argIndex,
		const std::string& message
	) :
		m_ID(warning),
		m_ArgIndex(argIndex),
		m_Message(message)
	{
	}

	ValidationFailure::ValidationFailure(
		PublicErrorID error,
		size_t argIndex,
		const std::string& message
	) :
		m_ID(error),
		m_ArgIndex(argIndex),
		m_Message(message)
	{
	}

	bool ValidationFailure::IsWarning() const
	{
		return std::holds_alternative<PublicWarningID>(m_ID);
	}

	bool ValidationFailure::IsError() const
	{
		return std::holds_alternative<PublicErrorID>(m_ID);
	}

	PublicWarningID ValidationFailure::GetWarningID() const
	{
		return IsWarning() ? std::get<PublicWarningID>(m_ID) : PublicWarningID::INTERNAL;
	}

	PublicErrorID ValidationFailure::GetErrorID() const
	{
		return IsError() ? std::get<PublicErrorID>(m_ID) : PublicErrorID::INTERNAL;
	}

	size_t ValidationFailure::GetArgIndex() const
	{
		return m_ArgIndex;
	}

	const std::string& ValidationFailure::GetMessage() const
	{
		return m_Message;
	}

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

	static ValidationFailure TooManyArgumentsFailure(size_t expected, size_t actual, size_t argIndex = 0)
	{
		return ValidationFailure(
					PublicWarningID::TOO_MANY_ARGUMENTS,
					argIndex,
					"Expected " + std::to_string(expected) +
					" arguments to instruction but got " + std::to_string(actual) +
					" arguments. Additional arguments ignored."
		);
	}

	static ValidationFailure TooFewArgumentsFailure(size_t expected, size_t actual, size_t argIndex = 0)
	{
		return ValidationFailure(
					PublicErrorID::TOO_FEW_ARGUMENTS,
					argIndex,
					"Expected " + std::to_string(expected) +
					" arguments to instruction but got " + std::to_string(actual) + " arguments."
		);
	}

	static ValidationFailure ArgumentOutOfRangeFailure(
		int32_t minValue,
		int32_t maxValue,
		int32_t origValue,
		int32_t newValue,
		size_t argIndex = 0)
	{
		return ValidationFailure(
					PublicWarningID::ARG_OUT_OF_RANGE,
					argIndex,
					"Value " + DecAndHexString(origValue) + " was out of valid range " +
					DecAndHexString(minValue) + " - " + DecAndHexString(maxValue) + ", truncating to " +
					DecAndHexString(newValue) + "."
		);
	}

	static ValidationFailure LabelRefValueOutOfRangeFailure(
		const std::string& labelName,
		int32_t minValue,
		int32_t maxValue,
		int32_t origValue,
		size_t argIndex = 0)
	{
		return ValidationFailure(
					PublicErrorID::LABEL_REF_OUT_OF_RANGE,
					argIndex,
					"Value " + DecAndHexString(origValue) + " for reference to label \"" + labelName +
					"\" was out of valid range " +
					DecAndHexString(minValue) + " - " + DecAndHexString(maxValue) + "."
		);
	}

	static ValidationFailure LabelRefUsedForReservedBitsFailure(
		size_t argIndex = 0)
	{
		return ValidationFailure(
			PublicErrorID::INVALID_ARGUMENT_TYPE,
			argIndex,
			"This argument is reserved in this context, and so must be set to zero. "
			"A label reference may not be used here."
		);
	}

	static ValidationFailure ReservedBitsSetFailure(
		size_t argIndex = 0)
	{
		return ValidationFailure(
			PublicWarningID::RESERVED_BITS_SET,
			argIndex,
			"This argument is reserved in this context. Its value will be set to zero."
		);
	}

	static std::vector<ValidationFailure> ValidateZeroArgCodeWord(const CodeWord& codeWord)
	{
		if ( codeWord.GetArgumentCount() > 0 )
		{
			return { TooManyArgumentsFailure(0, codeWord.GetArgumentCount()) };
		}

		return {};
	}

	static bool ValidateRegIdentifier(size_t argIndex, const CodeWordArg& arg, std::vector<ValidationFailure>& failures)
	{
		if ( !arg.IsNumber() )
		{
			failures.emplace_back(PublicErrorID::INVALID_REGISTER_ID, argIndex, "Expected numerical register identifier.");
			return false;
		}

		const int32_t value = arg.GetValue();

		if ( value < 0 || static_cast<uint32_t>(value) & ~(REG_ID_MASK) )
		{
			failures.emplace_back(PublicErrorID::INVALID_REGISTER_ID, argIndex, "Invalid register identifier.");
			return false;
		}

		return true;
	}

	static bool ValidateReservedArgIsZero(size_t argIndex, CodeWordArg& arg, std::vector<ValidationFailure>& failures)
	{
		if ( arg.IsLabelReference() )
		{
			// Label references are semantically never meant to be used in place of reserved bits.
			// The only reason a label reference would be zero is by chance, which does not excuse
			// it from being used here. If a label reference is present, throw an error.

			failures.emplace_back(LabelRefUsedForReservedBitsFailure(argIndex));
			return false;
		}

		if ( arg.GetValue() != 0 )
		{
			failures.emplace_back(ReservedBitsSetFailure(argIndex));
			arg.SetValue(0);
			return false;
		}

		return true;
	}

	static bool ValidateNumberForArg(
		size_t argIndex,
		const ArgMeta& argMeta,
		CodeWordArg& arg,
		std::vector<ValidationFailure>& failures
	)
	{
		const size_t numberOfBits = static_cast<size_t>(argMeta.highBit - argMeta.lowBit) + 1;

		const int32_t minValue = (argMeta.flags & ARGFLAG_SIGNED)
			? MinSignedValue(numberOfBits)
			: 0;

		const int32_t maxValue = (argMeta.flags & ARGFLAG_SIGNED)
			? MaxSignedValue(numberOfBits)
			: MaxUnsignedValue(numberOfBits);

		const int32_t actualValue = arg.GetValue();

		if ( actualValue >= minValue && actualValue <= maxValue )
		{
			return true;
		}

		if ( arg.IsLabelReference() )
		{
			// Error out here, since label refs are supposed to be used for jumping to specific locations
			// in code, and if that location is not correct then the developer is going to have a bad time.

			failures.emplace_back(
				LabelRefValueOutOfRangeFailure(
					arg.GetLabelReference().GetLabelName(),
					minValue,
					maxValue,
					actualValue,
					argIndex
				)
			);

			return false;
		}

		const uint32_t keptBits = static_cast<uint32_t>(actualValue) & BitMask(numberOfBits);
		int32_t newValue = 0;

		if ( actualValue >= 0 )
		{
			// Pad with leading zeroes.
			newValue = static_cast<int32_t>(keptBits);
		}
		else
		{
			// Pad with leading ones.
			newValue = static_cast<int32_t>((~BitMask(numberOfBits)) | keptBits);
		}

		arg.SetValue(newValue);
		failures.emplace_back(ArgumentOutOfRangeFailure(minValue, maxValue, actualValue, newValue));

		return false;
	}

	static std::vector<ValidationFailure> ValidateAddOrSub(InstructionType instructionType, CodeWord& codeWord, bool validateLabelRefs)
	{
		constexpr size_t EXPECTED_ARG_COUNT = 3;
		constexpr size_t ARG_SRC_REG = 0;
		constexpr size_t ARG_DEST_REG = 1;
		constexpr size_t ARG_VALUE = 2;

		const size_t expectedArgCount = GetInstructionMeta(instructionType).args.size();

		assert(expectedArgCount == EXPECTED_ARG_COUNT);

		const size_t actualArgCount = codeWord.GetArgumentCount();

		if ( actualArgCount < expectedArgCount )
		{
			return { TooFewArgumentsFailure(expectedArgCount, actualArgCount, std::max<size_t>(actualArgCount, 0)) };
		}

		std::vector<ValidationFailure> failures;

		if ( actualArgCount > expectedArgCount )
		{
			failures.emplace_back(TooManyArgumentsFailure(expectedArgCount, actualArgCount, expectedArgCount));
		}

		const CodeWordArg* srcRegArg = codeWord.GetArgument(ARG_SRC_REG);
		const CodeWordArg* destRegArg = codeWord.GetArgument(ARG_DEST_REG);
		CodeWordArg* valueArg = codeWord.GetArgument(ARG_VALUE);

		assert(srcRegArg && destRegArg && valueArg);

		if ( !ValidateRegIdentifier(ARG_SRC_REG, *srcRegArg, failures) || !ValidateRegIdentifier(ARG_DEST_REG, *destRegArg, failures) )
		{
			return failures;
		}

		if ( !valueArg->IsLabelReference() || validateLabelRefs )
		{
			if ( srcRegArg->GetValue() == destRegArg->GetValue() )
			{
				ValidateNumberForArg(ARG_VALUE, GetInstructionMeta(instructionType).args[ARG_VALUE], *valueArg, failures);
			}
			else
			{
				ValidateReservedArgIsZero(ARG_VALUE, *valueArg, failures);
			}
		}

		return failures;
	}

	std::vector<ValidationFailure> ValidateCodeWord(CodeWord& codeWord, bool validateLabelRefs)
	{
		const InstructionType instructionType = codeWord.GetInstructionType();

		switch ( instructionType )
		{
			case InstructionType::NOP:
			case InstructionType::SIG:
			{
				return ValidateZeroArgCodeWord(codeWord);
			}

			case InstructionType::ADD:
			case InstructionType::SUB:
			{
				return ValidateAddOrSub(instructionType, codeWord, validateLabelRefs);
			}

			default:
			{
				return {
					ValidationFailure(
						PublicErrorID::INTERNAL,
						0,
						"Unexpected instruction type encountered during code word validation."
					)
				};
			}
		}
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
