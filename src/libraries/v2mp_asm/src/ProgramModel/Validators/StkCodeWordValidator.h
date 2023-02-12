#pragma once

#include <memory>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class StkCodeWordValidator : public BasicCodeWordValidator
	{
	public:
		StkCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~StkCodeWordValidator();

	protected:
		void ValidateSpecific() override;
	};
}
