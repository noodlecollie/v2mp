#include <memory>
#include <stdexcept>
#include "ProgramModel/ProgramBuilder.h"
#include "ProgramModel/ProgramModel.h"

namespace V2MPAsm
{
	ProgramBuilder::ProgramBuilder() :
		m_ProgramModel(new ProgramModel())
	{
	}

	ProgramBuilder::~ProgramBuilder()
	{
	}

	CodeWord& ProgramBuilder::PrepareCodeWord(InstructionType instructionType)
	{
		m_CurrentCodeWord = std::make_shared<CodeWord>(instructionType);
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
	}

	void ProgramBuilder::SetNextLabelName(const std::string& labelName)
	{
		m_ProgramModel->SetNextLabelName(labelName);
	}

	bool ProgramBuilder::HasLabel(const std::string& labelName)
	{
		return m_ProgramModel->HasLabel(labelName);
	}

	std::optional<uint16_t> ProgramBuilder::GetLabelAddress(const std::string& labelName) const
	{
		return m_ProgramModel->GetLabelAddress(labelName);
	}

	std::unique_ptr<ProgramModel> ProgramBuilder::TakeProgramModel()
	{
		std::unique_ptr<ProgramModel> outModel = std::move(m_ProgramModel);
		m_ProgramModel = std::make_unique<ProgramModel>();
		return outModel;
	}
}
