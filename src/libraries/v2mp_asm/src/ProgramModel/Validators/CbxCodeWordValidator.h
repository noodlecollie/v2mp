#pragma once

#include <memory>
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	class CodeWord;

	class CbxCodeWordValidator : public BasicCodeWordValidator
	{
	public:
		CbxCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		virtual ~CbxCodeWordValidator();

	protected:
		void ValidateSpecific() override;
	};
}
