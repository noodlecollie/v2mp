#include <filesystem>
#include <fstream>
#include "Files/InputFile.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
	InputFile::InputFile(const std::string& path) :
		m_Path(path),
		m_Stream(m_Path)
	{
		if ( !m_Stream.good() )
		{
			throw AssemblerException(PublicErrorID::ERROR_OPENING_FILE, m_Path);
		}
	}

	std::string InputFile::GetPath() const
	{
		return m_Path;
	}

	std::ifstream& InputFile::GetStream()
	{
		return m_Stream;
	}

	size_t InputFile::CalculateFileSize()
	{
		std::ifstream::pos_type currentPos = m_Stream.tellg();
		m_Stream.seekg(0, m_Stream.end);
		std::ifstream::pos_type endPos = m_Stream.tellg();
		m_Stream.seekg(currentPos, m_Stream.beg);

		return static_cast<size_t>(endPos);
	}
}
