#include <cassert>
#include "ProgramModel/Validators/AddSubCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	AddSubCodeWordValidator::AddSubCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BaseCodeWordValidator(codeWord)
	{
	}

	AddSubCodeWordValidator::~AddSubCodeWordValidator()
	{
	}

	void AddSubCodeWordValidator::RunValidation()
	{
		constexpr size_t EXPECTED_ARG_COUNT = 3;
		constexpr size_t ARG_SRC_REG = 0;
		constexpr size_t ARG_DEST_REG = 1;
		constexpr size_t ARG_VALUE = 2;

		CodeWord& codeWord = GetCodeWord();

		const size_t expectedArgCount = GetInstructionMeta(codeWord.GetInstructionType()).args.size();
		assert(expectedArgCount == EXPECTED_ARG_COUNT);

		if ( !ValidateArgCount() )
		{
			return;
		}

		const CodeWordArg* srcRegArg = codeWord.GetArgument(ARG_SRC_REG);
		const CodeWordArg* destRegArg = codeWord.GetArgument(ARG_DEST_REG);
		CodeWordArg* valueArg = codeWord.GetArgument(ARG_VALUE);

		assert(srcRegArg && destRegArg && valueArg);

		if ( !ValidateRegIdentifier(ARG_SRC_REG) ||
		     !ValidateRegIdentifier(ARG_DEST_REG) )
		{
			return;
		}

		if ( !valueArg->IsLabelReference() || GetValidateLabelRefs() )
		{
			if ( srcRegArg->GetValue() == destRegArg->GetValue() )
			{
				ValidateNumberForArg(ARG_VALUE);
			}
			else
			{
				ValidateReservedArgIsZero(ARG_VALUE);
			}
		}
	}
}
