#pragma once

#include <memory>
#include <optional>
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

		void SetNextLabelName(const std::string& labelName);
		bool HasLabel(const std::string& labelName);
		std::optional<uint16_t> GetLabelAddress(const std::string& labelName) const;

		std::unique_ptr<ProgramModel> TakeProgramModel();

	private:
		std::unique_ptr<ProgramModel> m_ProgramModel;
		std::shared_ptr<CodeWord> m_CurrentCodeWord;
	};
}
