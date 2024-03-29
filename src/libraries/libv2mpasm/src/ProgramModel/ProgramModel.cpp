#include "ProgramModel/ProgramModel.h"
#include <memory>
#include <optional>
#include <stdexcept>
#include <limits>

namespace V2MPAsm
{
	static constexpr size_t MAX_CODE_WORD_LIST_SIZE = static_cast<size_t>(std::numeric_limits<uint16_t>::max()) / 2;

	void ProgramModel::AddCodeWord(const std::shared_ptr<CodeWord>& codeWord)
	{
		if ( !codeWord )
		{
			return;
		}

		if ( m_CodeWords.size() >= MAX_CODE_WORD_LIST_SIZE )
		{
			throw std::runtime_error("Program address space was exceeded.");
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
			throw std::logic_error("Cannot add label for non-existent code word.");
		}

		m_Labels.insert({labelName, m_CodeWords.back()});
	}

	std::shared_ptr<CodeWord> ProgramModel::CodeWordForLabel(const std::string& labelName) const
	{
		const LabelMap::const_iterator it = m_Labels.find(labelName);
		return it != m_Labels.end() ? it->second : std::shared_ptr<CodeWord>();
	}

	int32_t ProgramModel::GetValueOfLabelReference(const CodeWord& source, const CodeWord& target, LabelReference::ReferenceType refType) const
	{
		const uint16_t targetAddress = target.GetAddress();

		switch ( refType )
		{
			case LabelReference::ReferenceType::UPPER_BYTE_OF_ADDRESS:
			{
				return static_cast<int32_t>((targetAddress & 0xFF00) >> 8);
			}

			case LabelReference::ReferenceType::LOWER_BYTE_OF_ADDRESS:
			{
				return static_cast<int32_t>(targetAddress & 0x00FF);
			}

			case LabelReference::ReferenceType::SIGNED_NUM_WORDS_DIST_TO_LABEL:
			{
				// + sizeof(uint16_t) because the distance is measured from the address after the current word.
				const int32_t sourceAddress = static_cast<int32_t>(source.GetAddress()) + sizeof(uint16_t);
				const int32_t differenceInBytes = static_cast<int32_t>(targetAddress) - sourceAddress;

				return differenceInBytes / sizeof(uint16_t);
			}

			case LabelReference::ReferenceType::UNSIGNED_NUM_WORDS_DIST_TO_LABEL:
			{
				// + sizeof(uint16_t) because the distance is measured from the address after the current word.
				const int32_t sourceAddress = static_cast<int32_t>(source.GetAddress()) + sizeof(uint16_t);
				const int32_t targetAddressInt = static_cast<int32_t>(targetAddress);

				const int32_t differenceInBytes = sourceAddress > targetAddressInt
					? sourceAddress - targetAddressInt
					: targetAddressInt - sourceAddress;

				return differenceInBytes / sizeof(uint16_t);
			}

			default:
			{
				throw std::invalid_argument("Unknown label reference type");
			}
		}
	}
}
