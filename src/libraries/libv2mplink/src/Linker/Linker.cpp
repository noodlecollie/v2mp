#include <stdlib.h>
#include "Linker/Linker.h"
#include "LibToolchainComponents/InputFile.h"
#include "LibToolchainComponents/OutputFile.h"
#include "LibToolchainComponents/FilePool.h"
#include "Exceptions/LinkerException.h"
#include "cJSON.h"

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

			std::vector<char> inputData;
			inputData.resize(inputFile->CalculateFileSize());
			inputFile->GetStream().read(inputData.data(), inputData.size());

			WriteDummyJSONHeader(outputFile, inputData.size());
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

	void Linker::WriteDummyJSONHeader(const std::shared_ptr<OutputFile>& outputFile, size_t pageSize)
	{
		cJSON* root = cJSON_CreateArray();

		cJSON* header = cJSON_CreateObject();
		cJSON_AddStringToObject(header, "identifier", "v2mpbinf");
		cJSON_AddNumberToObject(header, "version", 1.0);
		cJSON_AddItemToArray(root, header);

		cJSON* exe = cJSON_CreateObject();
		cJSON_AddStringToObject(exe, "type", "executable");
		cJSON_AddNumberToObject(exe, "stack_size", 0.0);
		cJSON_AddItemToArray(root, exe);

		cJSON* codePages = cJSON_CreateArray();
		cJSON* codePage = cJSON_CreateObject();
		cJSON_AddStringToObject(codePage, "type", "code_page");
		cJSON_AddNumberToObject(codePage, "index", 0.0);
		cJSON_AddNumberToObject(codePage, "begin_file_offset_bytes", 0.0);
		cJSON_AddNumberToObject(codePage, "page_size_bytes", static_cast<double>(pageSize));
		cJSON_AddItemToArray(codePages, codePage);
		cJSON_AddItemToObject(exe, "code_pages", codePages);

		cJSON* entryPoint = cJSON_CreateObject();
		cJSON_AddStringToObject(codePage, "type", "entry_point");
		cJSON_AddNumberToObject(codePage, "code_page_index", 0.0);
		cJSON_AddNumberToObject(codePage, "code_address", 0.0);
		cJSON_AddItemToObject(exe, "entry_point", entryPoint);

		char* jsonString = cJSON_Print(root);

		if ( jsonString )
		{
			char* cursor = jsonString;

			while ( true )
			{
				outputFile->GetStream().put(*cursor);

				if ( !(*cursor) )
				{
					break;
				}
				else
				{
					++cursor;
				}
			}
		}

		free(jsonString);
		cJSON_Delete(root);
	}
}
