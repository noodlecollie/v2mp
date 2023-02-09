#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class ShftCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		ShftCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~ShftCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
