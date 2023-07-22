#include <filesystem>
#include <fstream>
#include "LibToolchainComponents/OutputFile.h"

namespace LibToolchainComponents
{
	OutputFile::OutputFile(const std::string& path) :
		m_Path(path),
		m_Stream(m_Path)
	{
		if ( !m_Stream.good() )
		{
			throw std::runtime_error("Failed to open output file");
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
