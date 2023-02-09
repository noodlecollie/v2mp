#pragma once

#include <memory>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class LdstCodeWordValidator : public BasicCodeWordValidator
	{
	public:
		LdstCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~LdstCodeWordValidator();

	protected:
		void ValidateSpecific() override;
	};
}
