#include "ProgramModel/Validators/CbxCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	CbxCodeWordValidator::CbxCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BaseCodeWordValidator(codeWord)
	{
	}

	CbxCodeWordValidator::~CbxCodeWordValidator()
	{
	}

	void CbxCodeWordValidator::RunValidation()
	{
		constexpr size_t EXPECTED_ARG_COUNT = 3;
		constexpr size_t ARG_PC_MOD_MODE = 0;
		constexpr size_t ARG_CONDITION = 1;
		constexpr size_t ARG_OFFSET = 2;

		CodeWord& codeWord = GetCodeWord();

		if ( !ValidateArgIsNumber(ARG_PC_MOD_MODE) ||
		     !ValidateArgIsNumber(ARG_CONDITION) ||
		     !ValidateNumberForArg(ARG_PC_MOD_MODE) ||
		     !ValidateNumberForArg(ARG_CONDITION) ||
		     !ValidateArgIsNumber(ARG_OFFSET) )
		{
			return;
		}

		const CodeWordArg& pcModModeArg = codeWord.GetArgumentRef(ARG_PC_MOD_MODE);

		if ( pcModModeArg.GetValue() != 0 )
		{
			ValidateReservedArgIsZero(ARG_OFFSET);
			return;
		}

		ValidateNumberForArg(ARG_OFFSET);
	}
}
