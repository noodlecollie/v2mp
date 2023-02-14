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

	std::shared_ptr<CodeWord> ProgramBuilder::GetCurrentCodeWordPtr()
	{
		return m_CurrentCodeWord;
	}

	void ProgramBuilder::SubmitCurrentCodeWord()
	{
		m_ProgramModel->AddCodeWord(m_CurrentCodeWord);
		m_CurrentCodeWord = std::make_shared<CodeWord>();

		SubmitQueuedLabels();
	}

	const std::vector<std::string>& ProgramBuilder::EnqueuedLabels() const
	{
		return m_QueuedLabels;
	}

	bool ProgramBuilder::HasEnqueuedLabels() const
	{
		return !m_QueuedLabels.empty();
	}

	void ProgramBuilder::EnqueueNextLabel(const std::string& labelName)
	{
		m_QueuedLabels.emplace_back(labelName);
	}

	void ProgramBuilder::EnqueueNextLabel(std::string&& labelName)
	{
		m_QueuedLabels.emplace_back(std::move(labelName));
	}

	bool ProgramBuilder::HasLabel(const std::string& labelName)
	{
		return m_ProgramModel->CodeWordForLabel(labelName) || LabelIsEnqueued(labelName);
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

	const ProgramModel& ProgramBuilder::GetProgramModel() const
	{
		if ( !m_ProgramModel )
		{
			throw std::runtime_error("GetProgramModel() called when no program model existed");
		}

		return *m_ProgramModel.get();
	}

	std::unique_ptr<ProgramModel> ProgramBuilder::TakeProgramModel()
	{
		std::unique_ptr<ProgramModel> outModel = std::move(m_ProgramModel);
		m_ProgramModel = std::make_unique<ProgramModel>();
		return outModel;
	}

	bool ProgramBuilder::LabelIsEnqueued(const std::string& labelName) const
	{
		return std::find(m_QueuedLabels.begin(), m_QueuedLabels.end(), labelName) != m_QueuedLabels.end();
	}

	void ProgramBuilder::SubmitQueuedLabels()
	{
		try
		{
			for ( const std::string& labelName : m_QueuedLabels )
			{
				m_ProgramModel->AddLabelForLastCodeWord(labelName);
			}

			m_QueuedLabels.clear();
		}
		catch ( const std::logic_error& ex )
		{
			m_QueuedLabels.clear();
			throw ex;
		}
	}
}
