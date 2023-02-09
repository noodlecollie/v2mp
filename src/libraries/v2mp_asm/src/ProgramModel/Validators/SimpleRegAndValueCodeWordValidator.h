#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class SimpleRegAndValueCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		SimpleRegAndValueCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~SimpleRegAndValueCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
