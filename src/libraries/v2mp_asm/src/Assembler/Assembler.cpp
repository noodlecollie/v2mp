#include <memory>
#include <filesystem>
#include "Assembler/Assembler.h"
#include "Exceptions/PublicException.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Interface_Exception.h"
#include "Parser/Parser.h"
#include "Files/FilePool.h"

namespace V2MPAsm
{
	bool Assembler::Run() noexcept
	{
		m_ExceptionList = RunInternal();
		return m_ExceptionList.empty();
	}

	void Assembler::SetInputFileName(const std::string& inFile)
	{
		m_InputFile = inFile;
	}

	void Assembler::SetOutputFileName(const std::string& outFile)
	{
		m_OutputFile = outFile;
	}

	const std::vector<std::shared_ptr<V2MPAsm_Exception>>& Assembler::GetExceptions() const noexcept
	{
		return m_ExceptionList;
	}

	ExceptionList Assembler::RunAndCompileExceptionList() noexcept
	{
		try
		{
			return RunInternal();
		}
		catch (const AssemblerException& ex)
		{
			return ExceptionList({ CreateException(ex.GetPublicException()) });
		}
		catch (...)
		{
			return ExceptionList({ CreateInternalErrorException(m_InputFile) });
		}
	}

	ExceptionList Assembler::RunInternal()
	{
		FilePool filePool;
		Parser parser(filePool.OpenInputFile(m_InputFile));
		return parser.ParseFile();
	}
}
