#include <cassert>
#include "ProgramModel/Validators/MulDivCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	MulDivCodeWordValidator::MulDivCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BaseCodeWordValidator(codeWord)
	{
	}

	MulDivCodeWordValidator::~MulDivCodeWordValidator()
	{
	}

	void MulDivCodeWordValidator::RunValidation()
	{
		constexpr size_t EXPECTED_ARG_COUNT = 4;
		constexpr size_t ARG_DEST_REG = 0;
		constexpr size_t ARG_SRC_MODE = 1;
		constexpr size_t ARG_SIGNEDNESS = 2;
		constexpr size_t ARG_VALUE = 3;

		CodeWord& codeWord = GetCodeWord();

		const size_t expectedArgCount = GetInstructionMeta(codeWord.GetInstructionType()).args.size();
		assert(expectedArgCount == EXPECTED_ARG_COUNT);

		if ( !ValidateArgCount() )
		{
			return;
		}

		const CodeWordArg* destRegArg = codeWord.GetArgument(ARG_DEST_REG);
		const CodeWordArg* srcModeArg = codeWord.GetArgument(ARG_SRC_MODE);
		const CodeWordArg* signednessArg = codeWord.GetArgument(ARG_SIGNEDNESS);
		CodeWordArg* valueArg = codeWord.GetArgument(ARG_VALUE);

		assert(destRegArg && srcModeArg && signednessArg && valueArg);

		if ( !ValidateRegIdentifier(ARG_DEST_REG, 0x1) ||
		     !ValidateNumberForArg(ARG_SRC_MODE) ||
		     !ValidateNumberForArg(ARG_SIGNEDNESS) )
		{
			return;
		}

		if ( srcModeArg->GetValue() != 0 )
		{
			// Literal value in operand
			ValidateNumberForArg(
				ARG_VALUE,
				signednessArg->GetValue() != 0
					? SignednessOverride::FORCE_SIGNED
					: SignednessOverride::FORCE_UNSIGNED
			);
		}
		else
		{
			// Operand must be zero
			ValidateReservedArgIsZero(ARG_VALUE);
		}
	}
}
