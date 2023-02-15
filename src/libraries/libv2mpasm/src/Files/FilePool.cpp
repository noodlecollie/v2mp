#include <filesystem>
#include <memory>
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Files/FilePool.h"

namespace V2MPAsm
{
	std::shared_ptr<InputFile> FilePool::OpenInputFile(const std::string& path)
	{
		if ( !std::filesystem::exists(path) )
		{
			throw AssemblerException(PublicErrorID::NON_EXISTENT_FILE, path);
		}

		const std::string canonicalPath = std::filesystem::canonical(path).string();
		const InputFileMap::iterator it = m_InputFileMap.find(canonicalPath);

		if ( it != m_InputFileMap.end() )
		{
			return it->second;
		}

		std::shared_ptr<InputFile> filePtr = std::make_shared<InputFile>(path);

		if ( !filePtr )
		{
			throw AssemblerException(PublicErrorID::ERROR_OPENING_FILE, path);
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
