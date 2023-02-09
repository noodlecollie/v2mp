#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class BitwCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		BitwCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~BitwCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
