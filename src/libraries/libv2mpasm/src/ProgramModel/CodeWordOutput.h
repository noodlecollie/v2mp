#pragma once

#include <ostream>
#include <cstdint>

namespace V2MPAsm
{
	class CodeWord;

	uint16_t ProduceCodeWord(const CodeWord& codeWord);
	void EmitCodeWordToStream(const CodeWord& codeWord, std::ostream& outStream);
}
