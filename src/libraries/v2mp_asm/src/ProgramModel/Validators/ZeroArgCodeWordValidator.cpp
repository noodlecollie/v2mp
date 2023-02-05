#include "ProgramModel/Validators/ZeroArgCodeWordValidator.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	ZeroArgCodeWordValidator::ZeroArgCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord) :
		BaseCodeWordValidator(codeWord)
	{
	}

	ZeroArgCodeWordValidator::~ZeroArgCodeWordValidator()
	{
	}

	void ZeroArgCodeWordValidator::RunValidation()
	{
		const size_t argCount = GetCodeWord().GetArgumentCount();

		if ( argCount > 0 )
		{
			AddFailure(CreateTooManyArgumentsFailure(0, argCount));
		}
	}
}
