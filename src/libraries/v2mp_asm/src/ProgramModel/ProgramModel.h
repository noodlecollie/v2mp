#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <stdexcept>
#include "ProgramModel/CodeWord.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
	class ProgramModel
	{
	public:
		void AddCodeWord(const std::shared_ptr<CodeWord>& codeWord);
		size_t GetCodeWordCount() const;
		std::shared_ptr<CodeWord> GetCodeWord(size_t index) const;

		void AddLabelForLastCodeWord(const std::string& labelName);
		std::shared_ptr<CodeWord> CodeWordForLabel(const std::string& labelName) const;

		int32_t GetValueOfLabelReference(const CodeWord& source, const CodeWord& target, LabelReference::ReferenceType refType) const;

	private:
		using LabelMap = std::unordered_map<std::string, std::shared_ptr<CodeWord>>;

		std::vector<std::shared_ptr<CodeWord>> m_CodeWords;
		LabelMap m_Labels;
	};
}
