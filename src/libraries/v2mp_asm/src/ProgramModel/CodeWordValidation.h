#pragma once

#include <vector>
#include <variant>
#include <string>
#include "Exceptions/PublicExceptionIDs.h"
#include "ProgramModel/ValidationFailure.h"

namespace V2MPAsm
{
	class CodeWord;
	class ProgramModel;

	std::vector<ValidationFailure> ValidateCodeWord(CodeWord& codeWord, bool validateLabelRefs = false);
}
