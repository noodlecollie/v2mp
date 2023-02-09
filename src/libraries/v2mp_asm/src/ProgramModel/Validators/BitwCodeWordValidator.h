#pragma once

#include <memory>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class BitwCodeWordValidator : public BasicCodeWordValidator
	{
	public:
		BitwCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~BitwCodeWordValidator();

	protected:
		void ValidateSpecific() override;
	};
}
