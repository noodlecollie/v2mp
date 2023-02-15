#include <filesystem>
#include <fstream>
#include "Files/OutputFile.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
	OutputFile::OutputFile(const std::string& path) :
		m_Path(path),
		m_Stream(m_Path)
	{
		if ( !m_Stream.good() )
		{
			throw AssemblerException(PublicErrorID::ERROR_OPENING_FILE, m_Path);
		}
	}

	std::string OutputFile::GetPath() const
	{
		return m_Path;
	}

	std::ofstream& OutputFile::GetStream()
	{
		return m_Stream;
	}
}
