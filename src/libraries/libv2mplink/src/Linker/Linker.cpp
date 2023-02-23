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

	V2MPLinker_LinkerResult Linker::Run() noexcept
	{
		m_ExceptionList = { std::make_shared<V2MPLink_Exception>(V2MPLink_Exception{PublicException(PublicErrorID::INTERNAL, m_ObjectFilePath, "Not implemented")}) };
		return V2MPLINK_FAILED;
	}
}
