#include "ProgramModel/ProgramModel.h"

namespace V2MPAsm
{
	void ProgramModel::AddCodeWord(CodeWord&& codeWord)
	{
		m_CodeWords.emplace_back(std::move(codeWord));
	}
}
