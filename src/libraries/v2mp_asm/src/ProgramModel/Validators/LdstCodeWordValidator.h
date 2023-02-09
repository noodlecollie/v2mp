#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class LdstCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		LdstCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~LdstCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
