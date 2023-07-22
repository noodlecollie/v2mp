#include <filesystem>
#include <memory>
#include <stdexcept>
#include "LibToolchainComponents/FilePool.h"

namespace LibToolchainComponents
{
	std::shared_ptr<InputFile> FilePool::OpenInputFile(const std::string& path)
	{
		if ( !std::filesystem::exists(path) )
		{
			throw std::runtime_error("Input file does not exist");
		}

		const std::string canonicalPath = std::filesystem::canonical(path).string();
		const InputFileMap::iterator it = m_InputFileMap.find(canonicalPath);

		if ( it != m_InputFileMap.end() )
		{
			return it->second;
		}

		std::shared_ptr<InputFile> filePtr = std::make_shared<InputFile>(path);

		// If the file itself could not be opened, an exception will already
		// have been thrown. If instead the pointer is null, something more
		// serious has happened.
		if ( !filePtr )
		{
			throw std::runtime_error("Internal error opening input file");
		}

		m_InputFileMap.insert({canonicalPath, filePtr});

		return filePtr;
	}

	void FilePool::CloseInputFile(const std::string& path)
	{
		m_InputFileMap.erase(std::filesystem::canonical(path).string());
	}

	bool FilePool::IsInputFileOpen(const std::string& path) const
	{
		return m_InputFileMap.find(std::filesystem::canonical(path).string()) != m_InputFileMap.end();
	}
}
