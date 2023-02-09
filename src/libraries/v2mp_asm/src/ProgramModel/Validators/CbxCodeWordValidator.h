#pragma once

#include <memory>
#include "ProgramModel/Validators/BaseCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class CbxCodeWordValidator : public BaseCodeWordValidator
	{
	public:
		CbxCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~CbxCodeWordValidator();

	protected:
		void RunValidation() override;
	};
}
