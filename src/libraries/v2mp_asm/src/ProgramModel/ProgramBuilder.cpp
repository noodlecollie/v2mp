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
		m_CurrentCodeWord = CodeWord(instructionType);
		return m_CurrentCodeWord;
	}

	CodeWord& ProgramBuilder::GetCurrentCodeWord()
	{
		return m_CurrentCodeWord;
	}

	void ProgramBuilder::SubmitCurrentCodeWord()
	{
		m_ProgramModel->AddCodeWord(std::move(m_CurrentCodeWord));
	}

	std::unique_ptr<ProgramModel> ProgramBuilder::TakeProgramModel()
	{
		std::unique_ptr<ProgramModel> outModel = std::move(m_ProgramModel);
		m_ProgramModel = std::make_unique<ProgramModel>();
		return outModel;
	}
}
