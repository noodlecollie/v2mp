#pragma once

#include <vector>
#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	class ProgramModel
	{
	public:
		void AddCodeWord(CodeWord&& codeWord);

	private:
		std::vector<CodeWord> m_CodeWords;
	};
}
