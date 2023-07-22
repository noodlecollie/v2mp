#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "LibToolchainComponents/InputFile.h"

namespace LibToolchainComponents
{
	InputFile::InputFile(const std::string& path) :
		m_Path(path),
		m_Stream(m_Path)
	{
		if ( !m_Stream.good() )
		{
			throw std::runtime_error("Failed to open input file");
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
