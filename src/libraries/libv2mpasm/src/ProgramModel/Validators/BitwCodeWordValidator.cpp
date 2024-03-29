#include "ProgramModel/Validators/BitwCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	BitwCodeWordValidator::BitwCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BasicCodeWordValidator(codeWord)
	{
	}

	BitwCodeWordValidator::~BitwCodeWordValidator()
	{
	}

	void BitwCodeWordValidator::ValidateSpecific()
	{
		constexpr size_t ARG_SRC_REG = 0;
		constexpr size_t ARG_DEST_REG = 1;
		constexpr size_t ARG_OP_TYPE = 2;
		constexpr size_t ARG_SHIFT_MAGNITUDE = 3;
		constexpr size_t ARG_INVERT_MASK = 4;

		CodeWord& codeWord = GetCodeWord();

		if ( !ValidateRegIdentifier(ARG_SRC_REG) ||
		     !ValidateRegIdentifier(ARG_DEST_REG) ||
		     !ValidateNumberForArg(ARG_OP_TYPE) )
		{
			return;
		}

		const CodeWordArg& srcRegArg = codeWord.GetArgumentRef(ARG_SRC_REG);
		const CodeWordArg& destRegArg = codeWord.GetArgumentRef(ARG_DEST_REG);

		if ( srcRegArg.GetValue() != destRegArg.GetValue() )
		{
			// Ensure the literal mask construction bits are zero.

			ValidateReservedArgIsZero(ARG_SHIFT_MAGNITUDE);
			ValidateReservedArgIsZero(ARG_INVERT_MASK);
			return;
		}

		if ( !ValidateNumberForArg(ARG_SHIFT_MAGNITUDE) || !ValidateNumberForArg(ARG_INVERT_MASK) )
		{
			return;
		}

		// Everything is OK.
	}
}
