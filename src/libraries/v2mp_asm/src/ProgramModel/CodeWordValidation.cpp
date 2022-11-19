#include "ProgramModel/CodeWordValidation.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/InstructionMeta.h"

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

		const int32_t value = arg.GetNumber();

		if ( value < 0 || static_cast<uint32_t>(value) & ~(REG_ID_MASK) )
		{
			failures.emplace_back(PublicErrorID::INVALID_REGISTER_ID, argIndex, "Invalid register identifier.");
			return false;
		}

		return true;
	}

	static bool ValidateAndClampArgRange(size_t argIndex, CodeWordArg& arg, int32_t min, int32_t max, std::vector<ValidationFailure>& failures)
	{
		if ( !arg.IsNumber() )
		{
			failures.emplace_back(PublicErrorID::INVALID_ARGUMENT_TYPE, argIndex, "Expected a numeric literal.");
			return false;
		}

		const int32_t value = arg.GetNumber();

		if ( value < min || value > max )
		{
			failures.emplace_back(
				PublicWarningID::ARG_OUT_OF_RANGE,
				argIndex,
				"Argument value " + std::to_string(value) + " is out of expected range [" + std::to_string(min) +
				" " + std::to_string(max) + "], clamping."
			);

			arg.SetNumber(std::max<int32_t>(min, std::min<int32_t>(value, max)));
		}

		return true;
	}

	static bool Validate8BitUnsignedArg(size_t argIndex, CodeWordArg& arg, std::vector<ValidationFailure>& failures)
	{
		return ValidateAndClampArgRange(argIndex, arg, 0, 255, failures);
	}

	static bool ValidateArgumentIsZero(size_t argIndex, CodeWordArg& arg, std::vector<ValidationFailure>& failures)
	{
		if ( !arg.IsNumber() )
		{
			failures.emplace_back(PublicErrorID::INVALID_ARGUMENT_TYPE, argIndex, "Expected a numeric literal.");
			return false;
		}

		const int32_t value = arg.GetNumber();

		if ( value != 0 )
		{
			failures.emplace_back(
				PublicWarningID::ARG_OUT_OF_RANGE,
				argIndex,
				"Forcing argument value to be zero."
			);

			arg.SetNumber(0);
		}

		return true;
	}

	static bool ValidateArgumentRanges(const InstructionMeta& meta, CodeWord& codeWord, std::vector<ValidationFailure>& failures)
	{
		const size_t expectedArgCount = meta.args.size();
		const size_t actualArgCount = codeWord.GetArgumentCount();

		if ( actualArgCount < expectedArgCount )
		{
			failures.emplace_back(
				TooFewArgumentsFailure(
					expectedArgCount,
					actualArgCount,
					std::max<size_t>(actualArgCount, 0)
				)
			);

			return false;
		}

		if ( actualArgCount > expectedArgCount )
		{
			// Only a warning, so don't return.
			failures.emplace_back(
				TooManyArgumentsFailure(
					expectedArgCount,
					actualArgCount,
					expectedArgCount
				)
			);
		}

		for ( size_t argIndex = 0; argIndex < actualArgCount; ++argIndex )
		{
			const ArgMeta& argMeta = meta.args[argIndex];

			if ( argMeta.signedness == ArgSignedness::DYNAMIC_SIGNEDNESS )
			{
				// Instruction-specific check, must happen later.
				continue;
			}

			const size_t numberOfBits = static_cast<size_t>(argMeta.highBit - argMeta.lowBit) + 1;

			const int32_t minValue = argMeta.signedness == ArgSignedness::ALWAYS_SIGNED
				? MinSignedValue(numberOfBits)
				: 0;

			const int32_t maxValue = argMeta.signedness == ArgSignedness::ALWAYS_SIGNED
				? MaxSignedValue(numberOfBits)
				: MaxUnsignedValue(numberOfBits);

			// TODO: Do we want to specify whether label references are allowed for an argument?
		}

		return true;
	}

	static std::vector<ValidationFailure> ValidateAddOrSub(CodeWord& codeWord)
	{
		constexpr size_t EXPECTED_ARG_COUNT = 3;
		const size_t actualArgCount = codeWord.GetArgumentCount();

		if ( actualArgCount < EXPECTED_ARG_COUNT )
		{
			return { TooFewArgumentsFailure(EXPECTED_ARG_COUNT, actualArgCount, std::max<size_t>(actualArgCount, 0)) };
		}

		if ( actualArgCount > EXPECTED_ARG_COUNT )
		{
			return { TooManyArgumentsFailure(EXPECTED_ARG_COUNT, actualArgCount, EXPECTED_ARG_COUNT) };
		}

		const CodeWordArg* srcRegArg = codeWord.GetArgument(0);
		const CodeWordArg* destRegArg = codeWord.GetArgument(1);
		CodeWordArg* valueArg = codeWord.GetArgument(2);

		std::vector<ValidationFailure> failures;

		if ( !ValidateRegIdentifier(0, *srcRegArg, failures) || !ValidateRegIdentifier(1, *destRegArg, failures) )
		{
			return failures;
		}

		if ( srcRegArg->GetNumber() == destRegArg->GetNumber() )
		{
			Validate8BitUnsignedArg(2, *valueArg, failures);
		}
		else
		{
			ValidateArgumentIsZero(2, *valueArg, failures);
		}

		return failures;
	}

	std::vector<ValidationFailure> ValidateCodeWord(CodeWord& codeWord)
	{
		switch ( codeWord.GetInstructionType() )
		{
			case InstructionType::NOP:
			case InstructionType::SIG:
			{
				return ValidateZeroArgCodeWord(codeWord);
			}

			case InstructionType::ADD:
			case InstructionType::SUB:
			{
				return ValidateAddOrSub(codeWord);
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
