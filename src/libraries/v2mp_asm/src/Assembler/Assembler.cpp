#include <memory>
#include <filesystem>
#include "Assembler/Assembler.h"
#include "Exceptions/PublicException.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Interface_Exception.h"
#include "Parser/Parser.h"
#include "Files/FilePool.h"
#include "Files/OutputFile.h"
#include "ProgramModel/CodeWordOutput.h"

namespace V2MPAsm
{
	bool Assembler::Run() noexcept
	{
		std::unique_ptr<ProgramModel> model;
		m_ExceptionList = ParseInputAndCompileExceptionList(model);

		if ( !m_ExceptionList.empty() )
		{
			return false;
		}

		m_ExceptionList = TryWriteOutputFile(std::move(model));
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

	ExceptionList Assembler::ParseInputAndCompileExceptionList(std::unique_ptr<ProgramModel>& model) noexcept
	{
		try
		{
			return ParseInput(model);
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

	ExceptionList Assembler::ParseInput(std::unique_ptr<ProgramModel>& model)
	{
		FilePool filePool;
		Parser parser;

		Parser::ParseResult result = parser.ParseFile(filePool.OpenInputFile(m_InputFile));

		if ( result.exceptions.size() < 1 )
		{
			model = std::move(result.programModel);
		}

		return result.exceptions;
	}

	ExceptionList Assembler::TryWriteOutputFile(std::unique_ptr<ProgramModel> model)
	{
		try
		{
			WriteOutputFile(model, std::make_shared<OutputFile>(m_OutputFile));
		}
		catch (const AssemblerException& ex)
		{
			return ExceptionList({ CreateException(ex.GetPublicException()) });
		}
		catch (...)
		{
			return ExceptionList({ CreateInternalErrorException(m_OutputFile) });
		}

		return ExceptionList();
	}

	void Assembler::WriteOutputFile(const std::unique_ptr<ProgramModel>& model, const std::shared_ptr<OutputFile>& outFile)
	{
		std::ostream& outStream = outFile->GetStream();
		const size_t codeWordCount = model->GetCodeWordCount();

		for ( size_t index = 0; index < codeWordCount; ++index )
		{
			try
			{

				EmitCodeWord(*model->GetCodeWord(index), outStream);
			}
			catch ( const std::runtime_error& ex )
			{
				throw AssemblerException(
					PublicErrorID::INTERNAL,
					outFile->GetPath(),
					LINE_NUMBER_BASE + index,
					COLUMN_NUMBER_BASE,
					ex.what()
				);
			}
		}
	}
}
