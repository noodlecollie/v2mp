#pragma once

#include <memory>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class AsgnCodeWordValidator : public BasicCodeWordValidator
	{
	public:
		AsgnCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~AsgnCodeWordValidator();

	protected:
		void ValidateSpecific() override;
	};
}
