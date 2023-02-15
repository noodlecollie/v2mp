#include "ProgramModel/Validators/AsgnCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	AsgnCodeWordValidator::AsgnCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BasicCodeWordValidator(codeWord)
	{
	}

	AsgnCodeWordValidator::~AsgnCodeWordValidator()
	{
	}

	void AsgnCodeWordValidator::ValidateSpecific()
	{
		constexpr size_t ARG_SRC_REG = 0;
		constexpr size_t ARG_DEST_REG = 1;
		constexpr size_t ARG_VALUE = 2;

		if ( !ValidateRegIdentifier(ARG_SRC_REG) ||
		     !ValidateRegIdentifier(ARG_DEST_REG) )
		{
			return;
		}

		CodeWord& codeWord = GetCodeWord();

		const CodeWordArg& srcRegArg = codeWord.GetArgumentRef(ARG_SRC_REG);
		const CodeWordArg& destRegArg = codeWord.GetArgumentRef(ARG_DEST_REG);
		CodeWordArg& valueArg = codeWord.GetArgumentRef(ARG_VALUE);

		if ( valueArg.IsLabelReference() && !GetValidateLabelRefs() )
		{
			return;
		}

		if ( srcRegArg.GetValue() != destRegArg.GetValue() )
		{
			// Literal value must always be zero.
			ValidateReservedArgIsZero(ARG_VALUE);
			return;
		}

		if ( srcRegArg.GetValue() == REG_ID_PC )
		{
			AddFailure(ValidationFailure(
				PublicErrorID::REGISTER_NOT_ALLOWED,
				ARG_SRC_REG,
				"Assigning to PC from literal value is not allowed - the value range is too small to be useful."
			));

			return;
		}

		ValidateNumberForArg(ARG_VALUE);
	}
}
