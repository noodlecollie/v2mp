#pragma once

#include <memory>
#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	class CodeWord;
	class BaseCodeWordValidator;

	std::unique_ptr<BaseCodeWordValidator> CreateValidator(const std::shared_ptr<CodeWord>& codeWord);
}
