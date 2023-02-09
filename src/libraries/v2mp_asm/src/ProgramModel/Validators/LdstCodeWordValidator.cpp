#include "ProgramModel/Validators/LdstCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	LdstCodeWordValidator::LdstCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BaseCodeWordValidator(codeWord)
	{
	}

	LdstCodeWordValidator::~LdstCodeWordValidator()
	{
	}

	void LdstCodeWordValidator::RunValidation()
	{
		constexpr size_t EXPECTED_ARG_COUNT = 2;
		constexpr size_t ARG_MODE = 0;
		constexpr size_t ARG_REG = 1;

		if ( !ValidateArgIsNumber(ARG_MODE) ||
		     !ValidateNumberForArg(ARG_MODE) ||
		     !ValidateRegIdentifier(ARG_REG) )
		{
			return;
		}

		// Everything is OK
	}
}
