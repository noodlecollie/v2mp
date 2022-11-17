#pragma once

#include <memory>
#include <optional>
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/InstructionMeta.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	class ProgramModel;

	class ProgramBuilder
	{
	public:
		ProgramBuilder();
		~ProgramBuilder();

		CodeWord& PrepareCodeWord(size_t line, size_t column, InstructionType instructionType);
		CodeWord& GetCurrentCodeWord();
		size_t GetCurrentCodeWordLine() const;
		size_t GetCurrentCodeWordColumn() const;
		void SubmitCurrentCodeWord();

		std::string GetNextLabelName() const;
		void SetNextLabelName(const std::string& labelName);
		bool HasLabel(const std::string& labelName);
		std::optional<uint16_t> GetLabelAddress(const std::string& labelName) const;

		std::unique_ptr<ProgramModel> TakeProgramModel();

	private:
		std::unique_ptr<ProgramModel> m_ProgramModel;
		std::shared_ptr<CodeWord> m_CurrentCodeWord;
		size_t m_CodeWordLine = LINE_NUMBER_BASE;
		size_t m_CodeWordColumn = COLUMN_NUMBER_BASE;
		std::string m_NextLabel;
	};
}
