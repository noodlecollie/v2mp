#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class AddSubCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		AddSubCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~AddSubCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
