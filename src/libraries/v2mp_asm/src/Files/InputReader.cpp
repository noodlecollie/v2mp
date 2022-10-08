#include "Files/InputReader.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
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

	size_t InputReader::GetPosition() const
	{
		return m_Position;
	}

	bool InputReader::IsEOF() const
	{
		return m_Position >= m_Data.size();
	}

	char InputReader::ReadChar()
	{
		return (!IsEOF()) ? m_Data[m_Position++] : '\0';
	}
}
