#include <stdexcept>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/ValidationUtils.h"

namespace V2MPAsm
{
	BaseCodeWordValidator::BaseCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		m_CodeWord(codeWord)
	{
		if ( !m_CodeWord )
		{
			throw std::invalid_argument("Expected non-null code word to validate");
		}
	}

	BaseCodeWordValidator::~BaseCodeWordValidator()
	{
	}

	CodeWord& BaseCodeWordValidator::GetCodeWord()
	{
		return *m_CodeWord;
	}

	const CodeWord& BaseCodeWordValidator::GetCodeWord() const
	{
		return *m_CodeWord;
	}

	bool BaseCodeWordValidator::GetValidateLabelRefs() const
	{
		return m_ValidateLabelRefs;
	}

	void BaseCodeWordValidator::SetValidateLabelRefs(bool validate)
	{
		m_ValidateLabelRefs = validate;
	}

	bool BaseCodeWordValidator::Validate()
	{
		m_ValidationFailures.clear();

		RunValidation();
		return m_ValidationFailures.empty();
	}

	const std::vector<ValidationFailure>& BaseCodeWordValidator::GetValidationFailures() const
	{
		return m_ValidationFailures;
	}

	bool BaseCodeWordValidator::ValidateRegIdentifier(size_t argIndex)
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

		if ( !arg->IsNumber() )
		{
			AddFailure(ValidationFailure(
				PublicErrorID::INVALID_REGISTER_ID,
				argIndex,
				"Expected numerical register identifier."
			));

			return false;
		}

		const int32_t value = arg->GetValue();

		if ( value < 0 || static_cast<uint32_t>(value) & ~(REG_ID_MASK) )
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

	bool BaseCodeWordValidator::ValidateReservedArgIsZero(size_t argIndex)
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

	bool BaseCodeWordValidator::ValidateNumberForArg(size_t argIndex)
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
		const size_t numberOfBits = static_cast<size_t>(argMeta.highBit - argMeta.lowBit) + 1;

		const int32_t minValue = (argMeta.flags & ARGFLAG_SIGNED)
			? MinSignedValue(numberOfBits)
			: 0;

		const int32_t maxValue = (argMeta.flags & ARGFLAG_SIGNED)
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

	void BaseCodeWordValidator::AddFailure(const ValidationFailure& failure)
	{
		m_ValidationFailures.emplace_back(failure);
	}

	void BaseCodeWordValidator::AddFailure(ValidationFailure&& failure)
	{
		m_ValidationFailures.emplace_back(std::move(failure));
	}
}
