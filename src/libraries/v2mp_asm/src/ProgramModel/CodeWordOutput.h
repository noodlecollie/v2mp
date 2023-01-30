#pragma once

#include <ostream>

namespace V2MPAsm
{
	class CodeWord;

	void EmitCodeWord(const CodeWord& codeWord, std::ostream& outStream);
}
