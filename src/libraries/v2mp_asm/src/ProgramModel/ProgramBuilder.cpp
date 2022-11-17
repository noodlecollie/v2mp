#include <memory>
#include <optional>
#include <stdexcept>
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/ProgramBuilder.h"
#include "ProgramModel/ProgramModel.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	ProgramBuilder::ProgramBuilder() :
		m_ProgramModel(new ProgramModel())
	{
	}

	ProgramBuilder::~ProgramBuilder()
	{
	}

	CodeWord& ProgramBuilder::PrepareCodeWord(size_t line, size_t column, InstructionType instructionType)
	{
		m_CurrentCodeWord = std::make_shared<CodeWord>(instructionType, line, column);

		return *m_CurrentCodeWord;
	}

	CodeWord& ProgramBuilder::GetCurrentCodeWord()
	{
		return *m_CurrentCodeWord;
	}

	void ProgramBuilder::SubmitCurrentCodeWord()
	{
		m_ProgramModel->AddCodeWord(m_CurrentCodeWord);

		m_CurrentCodeWord = std::make_shared<CodeWord>();

		if ( !m_NextLabel.empty() )
		{
			m_ProgramModel->AddLabelForLastCodeWord(m_NextLabel);
			m_NextLabel.clear();
		}
	}

	std::string ProgramBuilder::GetNextLabelName() const
	{
		return m_NextLabel;
	}

	void ProgramBuilder::SetNextLabelName(const std::string& labelName)
	{
		m_NextLabel = labelName;
	}

	bool ProgramBuilder::HasLabel(const std::string& labelName)
	{
		return m_ProgramModel->CodeWordForLabel(labelName) || labelName == m_NextLabel;
	}

	std::optional<uint16_t> ProgramBuilder::GetLabelAddress(const std::string& labelName) const
	{
		const std::shared_ptr<CodeWord> codeWord = m_ProgramModel->CodeWordForLabel(labelName);

		if ( codeWord )
		{
			return codeWord->GetAddress();
		}

		return std::optional<uint16_t>();
	}

	std::unique_ptr<ProgramModel> ProgramBuilder::TakeProgramModel()
	{
		std::unique_ptr<ProgramModel> outModel = std::move(m_ProgramModel);
		m_ProgramModel = std::make_unique<ProgramModel>();
		return outModel;
	}
}
