#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class MulDivCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		MulDivCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~MulDivCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
