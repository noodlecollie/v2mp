#include "ProgramModel/Validators/SimpleRegAndValueCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	SimpleRegAndValueCodeWordValidator::SimpleRegAndValueCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BasicCodeWordValidator(codeWord)
	{
	}

	SimpleRegAndValueCodeWordValidator::~SimpleRegAndValueCodeWordValidator()
	{
	}

	void SimpleRegAndValueCodeWordValidator::ValidateSpecific()
	{
		constexpr size_t EXPECTED_ARG_COUNT = 3;
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
			ValidateReservedArgIsZero(ARG_VALUE);
			return;
		}

		ValidateNumberForArg(ARG_VALUE);
	}
}
