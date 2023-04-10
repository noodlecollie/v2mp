#include "Linker/Linker.h"
#include "LibToolchainComponents/InputFile.h"
#include "LibToolchainComponents/OutputFile.h"
#include "LibToolchainComponents/FilePool.h"
#include "Exceptions/LinkerException.h"

namespace V2MPLink
{
	static V2MPLinker_LinkerResult ResultFromExceptionList(const ExceptionList& list)
	{
		if ( list.empty() )
		{
			return V2MPLINK_COMPLETED_OK;
		}

		for ( const std::shared_ptr<V2MPLink_Exception>& ex : list )
		{
			if ( ex->inner.GetType() == V2MPLINK_EXCEPTION_ERROR )
			{
				return V2MPLINK_FAILED;
			}
		}

		return V2MPLINK_COMPLETED_WITH_WARNINGS;
	}

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
		m_ExceptionList = RunLinkProcess();
		return ResultFromExceptionList(m_ExceptionList);
	}

	ExceptionList Linker::RunLinkProcess()
	{
		try
		{
			LibToolchainComponents::FilePool filePool;
			std::shared_ptr<InputFile> inputFile = TryOpenInputFile(filePool, m_ObjectFilePath);
			std::shared_ptr<OutputFile> outputFile = TryOpenOutputFile(m_OutputPath);

			// TODO: For now, just copy output verbatim so we can verify this all works.
			// This will grow more sophisticated as the implementation progresses.
			std::vector<char> inputData;

			inputData.resize(inputFile->CalculateFileSize());
			inputFile->GetStream().read(inputData.data(), inputData.size());

			outputFile->GetStream().write(inputData.data(), inputData.size());

			return ExceptionList();
		}
		catch ( const LinkerException& ex )
		{
			return ExceptionList({ CreateException(ex.GetPublicException()) });
		}
		catch ( ... )
		{
			return ExceptionList({ CreateInternalErrorException(GetOutputPath()) });
		}
	}

	std::shared_ptr<Linker::InputFile> Linker::TryOpenInputFile(FilePool& filePool, const std::string& path)
	{
		try
		{
			return filePool.OpenInputFile(path);
		}
		catch ( const std::runtime_error& )
		{
			throw LinkerException(PublicErrorID::NON_EXISTENT_FILE, path);
		}
	}

	std::shared_ptr<Linker::OutputFile> Linker::TryOpenOutputFile(const std::string& path)
	{
		std::shared_ptr<Linker::OutputFile> ptr = std::make_shared<Linker::OutputFile>(path);

		if ( !ptr )
		{
			throw LinkerException(PublicErrorID::ERROR_OPENING_FILE, path);
		}

		return ptr;
	}
}
