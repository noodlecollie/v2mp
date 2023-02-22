#include "Linker/Linker.h"

namespace V2MPLink
{
	std::string Linker::GetObjectFilePath() const
	{
		return m_ObjectFilePath;
	}

	void Linker::SetObjectFilePath(const std::string& filePath)
	{
		m_ObjectFilePath = filePath;
	}

	std::string Linker::GetOutputPath() const
	{
		return m_OutputPath;
	}

	void Linker::SetOutputPath(const std::string& outputPath)
	{
		m_OutputPath = outputPath;
	}

	const ExceptionList& Linker::GetExceptions() const noexcept
	{
		return m_ExceptionList;
	}
}
