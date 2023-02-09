#pragma once

#include <memory>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class MulDivCodeWordValidator : public BasicCodeWordValidator
	{
	public:
		MulDivCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~MulDivCodeWordValidator();

	protected:
		void ValidateSpecific() override;
	};
}
