#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include "ProgramModel/CodeWord.h"

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

	private:
		using LabelMap = std::unordered_map<std::string, std::shared_ptr<CodeWord>>;

		std::vector<std::shared_ptr<CodeWord>> m_CodeWords;
		LabelMap m_Labels;
	};
}
