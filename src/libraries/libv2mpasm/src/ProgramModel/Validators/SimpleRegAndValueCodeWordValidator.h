#pragma once

#include <memory>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class SimpleRegAndValueCodeWordValidator : public BasicCodeWordValidator
	{
	public:
		SimpleRegAndValueCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~SimpleRegAndValueCodeWordValidator();

	protected:
		void ValidateSpecific() override;
	};
}
