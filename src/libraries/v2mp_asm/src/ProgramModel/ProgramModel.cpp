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

	uint16_t ProgramModel::GetValueOfLabelReference(const CodeWord& source, const CodeWord& target, LabelReference::ReferenceType refType) const
	{
		const uint16_t targetAddress = target.GetAddress();

		switch ( refType )
		{
			case LabelReference::ReferenceType::UPPER_BYTE_OF_ADDRESS:
			{
				return (targetAddress & 0xFF00) >> 8;
			}

			case LabelReference::ReferenceType::LOWER_BYTE_OF_ADDRESS:
			{
				return targetAddress & 0x00FF;
			}

			case LabelReference::ReferenceType::NUM_WORDS_DIST_TO_LABEL:
			{
				const uint16_t sourceAddress = source.GetAddress();

				const uint16_t differenceInBytes = sourceAddress > targetAddress
					? sourceAddress - targetAddress
					: targetAddress - sourceAddress;

				return differenceInBytes / sizeof(uint16_t);
			}

			default:
			{
				throw std::invalid_argument("Unknown label reference type");
			}
		}
	}
}
