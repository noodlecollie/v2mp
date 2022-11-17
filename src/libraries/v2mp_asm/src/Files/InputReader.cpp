#include <cassert>
#include "Files/InputReader.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	static inline bool StringContains(const std::string& str, char ch)
	{
		return str.find(ch) != std::string::npos;
	}

	InputReader::InputReader(const std::shared_ptr<InputFile>& file) :
		m_File(file)
	{
		if ( !file )
		{
			throw AssemblerException(PublicErrorID::INTERNAL);
		}

		m_Data.resize(m_File->CalculateFileSize());

		std::ifstream& stream = m_File->GetStream();
		stream.read(m_Data.data(), m_Data.size());
	}

	std::string InputReader::GetPath() const
	{
		return m_File->GetPath();
	}

	size_t InputReader::GetCurrentPosition() const
	{
		return m_Position;
	}

	size_t InputReader::GetCurrentLine() const
	{
		return m_Line;
	}

	size_t InputReader::GetCurrentColumn()
	{
		return m_Column;
	}

	size_t InputReader::GetDataSize() const
	{
		return m_Data.size();
	}

	bool InputReader::IsEOF() const
	{
		return m_Position >= m_Data.size();
	}

	char InputReader::ReadChar()
	{
		return m_Position < m_Data.size() ? AdvanceLineAndColumn() : '\0';
	}

	void InputReader::SkipChars(size_t maxCharsToSkip)
	{
		const size_t dataSize = m_Data.size();

		while ( maxCharsToSkip > 0 && m_Position < dataSize )
		{
			AdvanceLineAndColumn();
			--maxCharsToSkip;
		}
	}

	void InputReader::SkipForAnyOf(const std::string& charsToSkip)
	{
		const size_t dataSize = m_Data.size();

		while ( m_Position < dataSize )
		{
			if ( !StringContains(charsToSkip, m_Data[m_Position]) )
			{
				break;
			}

			AdvanceLineAndColumn();
		}
	}

	void InputReader::SkipForAnyOf(const std::function<bool(char)>& charIsSkippableFunc)
	{
		assert(charIsSkippableFunc);

		const size_t dataSize = m_Data.size();

		while ( m_Position < dataSize )
		{
			if ( !(charIsSkippableFunc(m_Data[m_Position])) )
			{
				break;
			}

			AdvanceLineAndColumn();
		}
	}

	void InputReader::SkipUntilAnyOf(const std::string& charsToSkip, bool advanceOnceReached)
	{
		const size_t dataSize = m_Data.size();

		while ( m_Position < dataSize )
		{
			if ( StringContains(charsToSkip, m_Data[m_Position]) )
			{
				break;
			}

			AdvanceLineAndColumn();
		}

		if ( advanceOnceReached && m_Position < dataSize )
		{
			AdvanceLineAndColumn();
		}
	}

	void InputReader::SkipUntilAnyOf(const std::function<bool(char)>& charIsNotSkippableFunc, bool advanceOnceReached)
	{
		assert(charIsNotSkippableFunc);

		const size_t dataSize = m_Data.size();

		while ( m_Position < dataSize )
		{
			if ( charIsNotSkippableFunc(m_Data[m_Position]) )
			{
				break;
			}

			AdvanceLineAndColumn();
		}

		if ( advanceOnceReached && m_Position < dataSize )
		{
			AdvanceLineAndColumn();
		}
	}

	const char* InputReader::GetDataAtCurrentPosition() const
	{
		return GetDataAtAbsolutePosition(m_Position);
	}

	const char* InputReader::GetDataAtAbsolutePosition(size_t position) const
	{
		return position < m_Data.size() ? m_Data.data() + position : nullptr;
	}

	char InputReader::PeekChar(size_t offset) const
	{
		offset += m_Position;

		return offset < m_Data.size() ? m_Data[offset] : '\0';
	}

	char InputReader::AdvanceLineAndColumn()
	{
		assert(m_Position < m_Data.size());

		const char ch = m_Data[m_Position++];

		if ( ch == '\n' )
		{
			// New line
			++m_Line;
			m_Column = COLUMN_NUMBER_BASE;
		}
		else
		{
			++m_Column;
		}

		return ch;
	}
}