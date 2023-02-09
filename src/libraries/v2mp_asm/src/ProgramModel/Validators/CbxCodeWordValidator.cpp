#include <cassert>
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

		const size_t expectedArgCount = GetInstructionMeta(codeWord.GetInstructionType()).args.size();
		assert(expectedArgCount == EXPECTED_ARG_COUNT);

		if ( !ValidateArgCount() )
		{
			return;
		}

		const CodeWordArg* pcModModeArg = codeWord.GetArgument(ARG_PC_MOD_MODE);
		assert(pcModModeArg);

		if ( !ValidateArgIsNumber(ARG_PC_MOD_MODE) ||
		     !ValidateArgIsNumber(ARG_CONDITION) ||
		     !ValidateNumberForArg(ARG_PC_MOD_MODE) ||
		     !ValidateNumberForArg(ARG_CONDITION) ||
		     !ValidateArgIsNumber(ARG_OFFSET) )
		{
			return;
		}

		if ( pcModModeArg->GetValue() != 0 )
		{
			ValidateReservedArgIsZero(ARG_OFFSET);
			return;
		}

		ValidateNumberForArg(ARG_OFFSET);
	}
}
