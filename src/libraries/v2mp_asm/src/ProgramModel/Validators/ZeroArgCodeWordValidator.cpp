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
		// Nothing to do, since everything is done by the base class.
	}
}
