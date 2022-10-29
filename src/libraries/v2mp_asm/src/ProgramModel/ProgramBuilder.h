#pragma once

#include <memory>
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	class ProgramModel;

	class ProgramBuilder
	{
	public:
		ProgramBuilder();
		~ProgramBuilder();

		CodeWord& PrepareCodeWord(InstructionType instructionType);
		CodeWord& GetCurrentCodeWord();
		void SubmitCurrentCodeWord();

		std::unique_ptr<ProgramModel> TakeProgramModel();

	private:
		std::unique_ptr<ProgramModel> m_ProgramModel;
		CodeWord m_CurrentCodeWord;
	};
}
