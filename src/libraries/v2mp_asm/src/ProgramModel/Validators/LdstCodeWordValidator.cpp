#include "ProgramModel/Validators/LdstCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	LdstCodeWordValidator::LdstCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BasicCodeWordValidator(codeWord)
	{
	}

	LdstCodeWordValidator::~LdstCodeWordValidator()
	{
	}

	void LdstCodeWordValidator::ValidateSpecific()
	{
		constexpr size_t EXPECTED_ARG_COUNT = 2;
		constexpr size_t ARG_MODE = 0;
		constexpr size_t ARG_REG = 1;

		if ( !ValidateNumberForArg(ARG_MODE) ||
		     !ValidateRegIdentifier(ARG_REG) )
		{
			return;
		}

		// Everything is OK
	}
}
