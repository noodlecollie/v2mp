#include <stdexcept>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/ValidationUtils.h"

namespace V2MPAsm
{
	BasicCodeWordValidator::BasicCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		m_CodeWord(codeWord)
	{
		if ( !m_CodeWord )
		{
			throw std::invalid_argument("Expected non-null code word to validate");
		}
	}

	BasicCodeWordValidator::~BasicCodeWordValidator()
	{
	}

	CodeWord& BasicCodeWordValidator::GetCodeWord()
	{
		return *m_CodeWord;
	}

	const CodeWord& BasicCodeWordValidator::GetCodeWord() const
	{
		return *m_CodeWord;
	}

	bool BasicCodeWordValidator::GetValidateLabelRefs() const
	{
		return m_ValidateLabelRefs;
	}

	void BasicCodeWordValidator::SetValidateLabelRefs(bool validate)
	{
		m_ValidateLabelRefs = validate;
	}

	BasicCodeWordValidator::ValidationResult BasicCodeWordValidator::Validate()
	{
		m_ValidationFailures.clear();

		if ( ValidateCommonArgConstraints() )
		{
			ValidateSpecific();
		}

		ValidationResult result = m_ValidationFailures.empty()
			? ValidationResult::VALID
			: ValidationResult::VALID_WITH_WARNINGS;

		for ( const ValidationFailure& failure : m_ValidationFailures )
		{
			if ( failure.IsError() )
			{
				result = ValidationResult::INVALID;
				break;
			}
		}

		return result;
	}

	const std::vector<ValidationFailure>& BasicCodeWordValidator::GetValidationFailures() const
	{
		return m_ValidationFailures;
	}

	bool BasicCodeWordValidator::ValidateCommonArgConstraints()
	{
		CodeWord& codeWord = GetCodeWord();
		const InstructionMeta& instructionMeta = GetInstructionMeta(codeWord.GetInstructionType());
		const size_t expectedArgCount = instructionMeta.args.size();
		const size_t actualArgCount = codeWord.GetArgumentCount();

		if ( actualArgCount < expectedArgCount )
		{
			AddFailure(CreateTooFewArgumentsFailure(
				expectedArgCount,
				actualArgCount,
				std::max<size_t>(actualArgCount, 0)
			));

			return false;
		}

		if ( actualArgCount > expectedArgCount )
		{
			// This is only a warning.
			AddFailure(CreateTooManyArgumentsFailure(expectedArgCount, actualArgCount, expectedArgCount));
		}

		bool invalidArgEncountered = false;

		for ( size_t index = 0; index < expectedArgCount; ++index )
		{
			const ArgMeta& argMeta = instructionMeta.args[index];
			const CodeWordArg& arg = codeWord.GetArgumentRef(index);

			if ( (argMeta.flags & ARGFLAG_SYMBOLIC) && arg.IsLabelReference() )
			{
				AddFailure(ValidationFailure(
					PublicErrorID::INVALID_REGISTER_ID,
					index,
					"Expected numerical register identifier."
				));

				invalidArgEncountered = true;
			}
		}

		return invalidArgEncountered;
	}

	bool BasicCodeWordValidator::ValidateRegIdentifier(size_t argIndex, uint32_t regIDMask)
	{
		CodeWordArg* arg = GetCodeWord().GetArgument(argIndex);

		if ( !arg )
		{
			AddFailure(ValidationFailure(
				PublicErrorID::INTERNAL,
				argIndex,
				"Expected argument at index " + std::to_string(argIndex) + "."
			));

			return false;
		}

		const int32_t value = arg->GetValue();

		if ( value < 0 || static_cast<uint32_t>(value) & ~(regIDMask) )
		{
			AddFailure(ValidationFailure(
				PublicErrorID::INVALID_REGISTER_ID,
				argIndex,
				"Invalid register identifier."
			));

			return false;
		}

		return true;
	}

	bool BasicCodeWordValidator::ValidateReservedArgIsZero(size_t argIndex)
	{
		CodeWordArg* arg = GetCodeWord().GetArgument(argIndex);

		if ( !arg )
		{
			AddFailure(ValidationFailure(
				PublicErrorID::INTERNAL,
				argIndex,
				"Expected argument at index " + std::to_string(argIndex) + "."
			));

			return false;
		}

		if ( arg->IsLabelReference() )
		{
			// Label references are semantically never meant to be used in place of reserved bits.
			// The only reason a label reference would be zero is by chance, which does not excuse
			// it from being used here. If a label reference is present, throw an error regardless
			// of whether we are validating label refs or not.

			AddFailure(CreateLabelRefUsedForReservedBitsFailure(argIndex));
			return false;
		}

		if ( arg->GetValue() != 0 )
		{
			AddFailure(CreateReservedBitsSetFailure(argIndex));
			arg->SetValue(0);

			return false;
		}

		return true;
	}

	bool BasicCodeWordValidator::ValidateNumberForArg(size_t argIndex, SignednessOverride signednessOverride)
	{
		CodeWordArg* arg = GetCodeWord().GetArgument(argIndex);

		if ( !arg )
		{
			AddFailure(ValidationFailure(
				PublicErrorID::INTERNAL,
				argIndex,
				"Expected argument at index " + std::to_string(argIndex) + "."
			));

			return false;
		}

		const std::vector<V2MPAsm::ArgMeta>& argMetaList = GetInstructionMeta(GetCodeWord().GetInstructionType()).args;

		if ( argIndex > argMetaList.size() )
		{
			AddFailure(ValidationFailure(
				PublicErrorID::INTERNAL,
				argIndex,
				"Argument index " + std::to_string(argIndex) + " was out of range for arg meta list."
			));

			return false;
		}

		const ArgMeta& argMeta = argMetaList[argIndex];

		if ( (argMeta.flags & ARGFLAG_DYNAMIC_SIGNEDNESS) && signednessOverride == SignednessOverride::NO_OVERRIDE )
		{
			// This should never happen if this function is being used correctly.
			AddFailure(ValidationFailure(
				PublicErrorID::INTERNAL,
				argIndex,
				"Argument index " + std::to_string(argIndex) + ": failed to resolve signedness."
			));

			return false;
		}

		const size_t numberOfBits = static_cast<size_t>(argMeta.highBit - argMeta.lowBit) + 1;

		bool argIsSigned = (argMeta.flags & ARGFLAG_SIGNED);

		if ( signednessOverride == SignednessOverride::FORCE_SIGNED )
		{
			argIsSigned = true;
		}
		else if ( signednessOverride == SignednessOverride::FORCE_UNSIGNED )
		{
			argIsSigned = false;
		}

		const int32_t minValue = argIsSigned
			? MinSignedValue(numberOfBits)
			: 0;

		const int32_t maxValue = argIsSigned
			? MaxSignedValue(numberOfBits)
			: MaxUnsignedValue(numberOfBits);

		const int32_t actualValue = arg->GetValue();

		if ( actualValue >= minValue && actualValue <= maxValue )
		{
			// Value is OK, no changes need to be applied.
			return true;
		}

		if ( arg->IsLabelReference() )
		{
			if ( GetValidateLabelRefs() )
			{
				// Error out here, since label refs are supposed to be used for jumping to specific locations
				// in code, and if that location is not correct because we clamped the value, then the
				// developer is going to have a bad time.

				AddFailure(
					CreateLabelRefValueOutOfRangeFailure(
						arg->GetLabelReference().GetLabelName(),
						minValue,
						maxValue,
						actualValue,
						argIndex
					)
				);

				return false;
			}
			else
			{
				// Let this validation failure be caught later on, once we know the label ref's value.
				return true;
			}
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

		arg->SetValue(newValue);
		AddFailure(CreateArgumentOutOfRangeFailure(minValue, maxValue, actualValue, newValue));

		return false;
	}

	void BasicCodeWordValidator::ValidateSpecific()
	{
		// Nothing to do in the base class.
	}

	void BasicCodeWordValidator::AddFailure(const ValidationFailure& failure)
	{
		m_ValidationFailures.emplace_back(failure);
	}

	void BasicCodeWordValidator::AddFailure(ValidationFailure&& failure)
	{
		m_ValidationFailures.emplace_back(std::move(failure));
	}
}
