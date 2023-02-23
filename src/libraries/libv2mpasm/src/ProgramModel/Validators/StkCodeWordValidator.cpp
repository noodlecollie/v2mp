#include "ProgramModel/Validators/StkCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	StkCodeWordValidator::StkCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BasicCodeWordValidator(codeWord)
	{
	}

	StkCodeWordValidator::~StkCodeWordValidator()
	{
	}

	void StkCodeWordValidator::ValidateSpecific()
	{
		constexpr size_t ARG_MODE = 0;
		constexpr size_t ARG_REGISTERS = 1;

		ValidateNumberForArg(ARG_MODE);
		ValidateNumberForArg(ARG_REGISTERS);

		if ( GetCodeWord().GetArgumentRef(ARG_REGISTERS).GetValue() == 0 )
		{
			AddFailure(ValidationFailure(
				PublicWarningID::INALID_ARG_VALUE,
				ARG_REGISTERS,
				"At least one register flag must be specified."
			));
	}
	}
}
