#pragma once

#include <vector>
#include <memory>
#include "ProgramModel/ValidationFailure.h"

namespace V2MPAsm
{
	class CodeWord;

	std::vector<ValidationFailure> ValidateCodeWord(const std::shared_ptr<CodeWord>& codeWord, bool validateLabelRefs = false);
}
