#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class AsgnCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		AsgnCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~AsgnCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
