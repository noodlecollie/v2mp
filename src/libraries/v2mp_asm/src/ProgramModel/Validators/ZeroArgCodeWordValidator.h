#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class ZeroArgCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		ZeroArgCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~ZeroArgCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
