#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include "Interface_Exception.h"

namespace V2MPAsm
{
	class OutputFile;
	class ProgramModel;

	class Assembler
	{
	public:
		void SetInputFileName(const std::string& inFile);
		void SetOutputFileName(const std::string& outFile);

		bool Run() noexcept;

		const ExceptionList& GetExceptions() const noexcept;

	private:
		ExceptionList ParseInputAndCompileExceptionList(std::unique_ptr<ProgramModel>& model) noexcept;
		ExceptionList ParseInput(std::unique_ptr<ProgramModel>& model);

		ExceptionList TryWriteOutputFile(std::unique_ptr<ProgramModel> model);
		void WriteOutputFile(const std::unique_ptr<ProgramModel>& model, const std::shared_ptr<OutputFile>& outFile);

		ExceptionList m_ExceptionList;
		std::string m_InputFile;
		std::string m_OutputFile;
	};
}
