#pragma once

#include <memory>
#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	class CodeWord;
	class BasicCodeWordValidator;

	std::unique_ptr<BasicCodeWordValidator> CreateValidator(const std::shared_ptr<CodeWord>& codeWord);
}
