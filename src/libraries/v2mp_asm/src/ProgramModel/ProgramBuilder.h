#pragma once

#include <memory>
#include <optional>
#include <vector>
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
		std::shared_ptr<CodeWord> GetCurrentCodeWordPtr();
		size_t GetCurrentCodeWordLine() const;
		size_t GetCurrentCodeWordColumn() const;
		void SubmitCurrentCodeWord();

		const std::vector<std::string>& EnqueuedLabels() const;
		bool HasEnqueuedLabels() const;

		void EnqueueNextLabel(const std::string& labelName);
		void EnqueueNextLabel(std::string&& labelName);
		bool HasLabel(const std::string& labelName);
		std::optional<uint16_t> GetLabelAddress(const std::string& labelName) const;

		const ProgramModel& GetProgramModel() const;
		std::unique_ptr<ProgramModel> TakeProgramModel();

	private:
		bool LabelIsEnqueued(const std::string& labelName) const;
		void SubmitQueuedLabels();

		std::unique_ptr<ProgramModel> m_ProgramModel;
		std::shared_ptr<CodeWord> m_CurrentCodeWord;

		// Having multiple labels before the same codeword is fine,
		// provided they're all different. We support this by keeping
		// a list of labels.
		std::vector<std::string> m_QueuedLabels;
	};
}
