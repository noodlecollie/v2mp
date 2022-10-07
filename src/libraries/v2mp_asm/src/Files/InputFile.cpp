#include <filesystem>
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
}
