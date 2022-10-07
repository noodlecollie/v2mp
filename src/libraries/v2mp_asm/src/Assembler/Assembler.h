#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include "Interface_Exception.h"

namespace V2MPAsm
{
	class Assembler
	{
	public:
		void SetInputFileName(const std::string& inFile);
		void SetOutputFileName(const std::string& outFile);

		bool Run() noexcept;

		const ExceptionList& GetExceptions() const noexcept;

	private:
		ExceptionList RunAndCompileExceptionList() noexcept;
		ExceptionList RunInternal();

		ExceptionList m_ExceptionList;
		std::string m_InputFile;
		std::string m_OutputFile;
	};
}
