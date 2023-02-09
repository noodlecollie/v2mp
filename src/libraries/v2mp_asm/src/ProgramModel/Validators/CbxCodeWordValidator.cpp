#include "ProgramModel/Validators/CbxCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	CbxCodeWordValidator::CbxCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BasicCodeWordValidator(codeWord)
	{
	}

	CbxCodeWordValidator::~CbxCodeWordValidator()
	{
	}

	void CbxCodeWordValidator::ValidateSpecific()
	{
		constexpr size_t EXPECTED_ARG_COUNT = 3;
		constexpr size_t ARG_PC_MOD_MODE = 0;
		constexpr size_t ARG_CONDITION = 1;
		constexpr size_t ARG_OFFSET = 2;

		CodeWord& codeWord = GetCodeWord();

		if ( !ValidateNumberForArg(ARG_PC_MOD_MODE) ||
		     !ValidateNumberForArg(ARG_CONDITION) )
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
