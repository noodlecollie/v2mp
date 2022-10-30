#include "ProgramModel/ProgramModel.h"
#include <optional>

namespace V2MPAsm
{
	void ProgramModel::AddCodeWord(const std::shared_ptr<CodeWord>& codeWord)
	{
		if ( !codeWord )
		{
			return;
		}

		if ( !m_NextLabel.empty() )
		{
			m_Labels.insert({m_NextLabel, codeWord});
			m_NextLabel.clear();
		}

		codeWord->SetAddress(m_CodeWords.size() * 2);

		m_CodeWords.emplace_back(codeWord);
	}

	std::string ProgramModel::GetNextLabelName() const
	{
		return m_NextLabel;
	}

	void ProgramModel::SetNextLabelName(const std::string& labelName)
	{
		m_NextLabel = labelName;
	}

	bool ProgramModel::HasLabel(const std::string& labelName) const
	{
		return m_NextLabel == labelName || m_Labels.find(labelName) != m_Labels.end();
	}

	std::optional<uint16_t> ProgramModel::GetLabelAddress(const std::string& labelName) const
	{
		auto it = m_Labels.find(labelName);

		if ( it != m_Labels.end() )
		{
			return it->second->GetAddress();
		}

		return std::optional<uint16_t>();
	}
}
