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

		std::string GetNextLabelName() const;
		void SetNextLabelName(const std::string& labelName);
		bool HasLabel(const std::string& labelName) const;
		std::optional<uint16_t> GetLabelAddress(const std::string& labelName) const;

	private:
		using LabelMap = std::unordered_map<std::string, std::shared_ptr<CodeWord>>;

		std::vector<std::shared_ptr<CodeWord>> m_CodeWords;
		LabelMap m_Labels;
		std::string m_NextLabel;
	};
}
