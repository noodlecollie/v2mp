#include "ProgramModel/ProgramModel.h"
#include <memory>
#include <optional>
#include <stdexcept>

namespace V2MPAsm
{
	void ProgramModel::AddCodeWord(const std::shared_ptr<CodeWord>& codeWord)
	{
		if ( !codeWord )
		{
			return;
		}

		codeWord->SetAddress(static_cast<uint16_t>(m_CodeWords.size() * 2));
		m_CodeWords.emplace_back(codeWord);
	}

	size_t ProgramModel::GetCodeWordCount() const
	{
		return m_CodeWords.size();
	}

	std::shared_ptr<CodeWord> ProgramModel::GetCodeWord(size_t index) const
	{
		return index < m_CodeWords.size() ? m_CodeWords[index] : std::shared_ptr<CodeWord>();
	}

	void ProgramModel::AddLabelForLastCodeWord(const std::string& labelName)
	{
		if ( m_CodeWords.empty() )
		{
			throw std::logic_error("No code words in list");
		}

		m_Labels.insert({labelName, m_CodeWords.back()});
	}

	std::shared_ptr<CodeWord> ProgramModel::CodeWordForLabel(const std::string& labelName) const
	{
		const LabelMap::const_iterator it = m_Labels.find(labelName);
		return it != m_Labels.end() ? it->second : std::shared_ptr<CodeWord>();
	}
}
