#include <memory>
#include <filesystem>
#include <cstring>
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
	void Assembler::SetInputFileName(const std::string& inFile) noexcept
	{
		m_Input = inFile;
	}

	void Assembler::SetInputRawData(const std::string& path, const char* data) noexcept
	{
		m_Input = InputRawData{ path, CopyInput(data) };
	}

	void Assembler::SetOutputToFile(const std::string& outFile) noexcept
	{
		m_Output = outFile;
	}

	void Assembler::SetOutputToRawData() noexcept
	{
		m_Output = std::vector<uint16_t>();
	}

	bool Assembler::Run() noexcept
	{
		std::unique_ptr<ProgramModel> model;
		m_ExceptionList = ParseInputAndCompileExceptionList(model);

		if ( !m_ExceptionList.empty() )
		{
			return false;
		}

		if ( OutputIsRawData() )
		{
			m_ExceptionList = TryWriteRawOutput(std::move(model));
		}
		else
		{
			m_ExceptionList = TryWriteOutputFile(std::move(model));
		}

		return m_ExceptionList.empty();
	}

	const std::vector<std::shared_ptr<V2MPAsm_Exception>>& Assembler::GetExceptions() const noexcept
	{
		return m_ExceptionList;
	}

	bool Assembler::InputIsRawData() const noexcept
	{
		return std::holds_alternative<InputRawData>(m_Input);
	}

	bool Assembler::OutputIsRawData() const noexcept
	{
		return std::holds_alternative<std::vector<uint16_t>>(m_Output);
	}

	std::vector<uint16_t> Assembler::TakeRawOutput() noexcept
	{
		return OutputIsRawData() ? std::move(std::get<std::vector<uint16_t>>(m_Output)) : std::vector<uint16_t>();
	}

	size_t Assembler::RawOutputSizeInWords() const noexcept
	{
		return OutputIsRawData() ? std::get<std::vector<uint16_t>>(m_Output).size() : 0;
	}

	std::string Assembler::GetInputPath() const
	{
		return InputIsRawData()
			? std::get<InputRawData>(m_Input).path
			: std::get<std::string>(m_Input);
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
			return ExceptionList({ CreateInternalErrorException(GetInputPath()) });
		}
	}

	ExceptionList Assembler::ParseInput(std::unique_ptr<ProgramModel>& model)
	{
		FilePool filePool;
		Parser parser;

		Parser::ParseResult result;

		if ( InputIsRawData() )
		{
			// TODO: This moves the input data, which we then cannot get back.
			// We should make it shared with the parser.
			InputRawData& rawData = std::get<InputRawData>(m_Input);
			parser.ParseData(rawData.path, std::move(rawData.data));
		}
		else
		{
			parser.ParseFile(filePool.OpenInputFile(GetInputPath()));
		}

		if ( result.exceptions.size() < 1 )
		{
			model = std::move(result.programModel);
		}

		return result.exceptions;
	}

	ExceptionList Assembler::TryWriteOutputFile(std::unique_ptr<ProgramModel> model)
	{
		std::string outPath;

		try
		{
			outPath = std::get<std::string>(m_Output);
			std::shared_ptr<OutputFile> outFile = std::make_shared<OutputFile>(outPath);

			WriteOutput(model, outPath, outFile->GetStream());
		}
		catch (const AssemblerException& ex)
		{
			return ExceptionList({ CreateException(ex.GetPublicException()) });
		}
		catch (...)
		{
			return ExceptionList({ CreateInternalErrorException(outPath) });
		}

		return ExceptionList();
	}

	ExceptionList Assembler::TryWriteRawOutput(std::unique_ptr<ProgramModel> model)
	{
		try
		{
			WriteOutput(model, std::get<std::vector<uint16_t>>(m_Output));
		}
		catch (const AssemblerException& ex)
		{
			return ExceptionList({ CreateException(ex.GetPublicException()) });
		}
		catch (...)
		{
			return ExceptionList({ CreateInternalErrorException("<raw output>") });
		}

		return ExceptionList();
	}

	void Assembler::WriteOutput(const std::unique_ptr<ProgramModel>& model, const std::string& outPath, std::ostream& outStream)
	{
		const size_t codeWordCount = model->GetCodeWordCount();

		for ( size_t index = 0; index < codeWordCount; ++index )
		{
			try
			{
				EmitCodeWordToStream(*model->GetCodeWord(index), outStream);
			}
			catch ( const std::runtime_error& ex )
			{
				throw AssemblerException(
					PublicErrorID::INTERNAL,
					outPath,
					LINE_NUMBER_BASE + index,
					COLUMN_NUMBER_BASE,
					ex.what()
				);
			}
		}
	}

	void Assembler::WriteOutput(const std::unique_ptr<ProgramModel>& model, std::vector<uint16_t>& outVec)
	{
		const size_t codeWordCount = model->GetCodeWordCount();

		outVec.clear();
		outVec.reserve(codeWordCount);

		for ( size_t index = 0; index < codeWordCount; ++index )
		{
			try
			{
				outVec.push_back(ProduceCodeWord(*model->GetCodeWord(index)));
			}
			catch ( const std::runtime_error& ex )
			{
				throw AssemblerException(
					PublicErrorID::INTERNAL,
					"<raw output>",
					LINE_NUMBER_BASE + index,
					COLUMN_NUMBER_BASE,
					ex.what()
				);
			}
		}
	}

	std::vector<char> Assembler::CopyInput(const char* input)
	{
		if ( !input )
		{
			return std::vector<char>();
		}

		const size_t length = std::strlen(input) + 1;
		std::vector<char> out(length, '\0');

		std::memcpy(out.data(), input, length);

		return out;
	}
}
