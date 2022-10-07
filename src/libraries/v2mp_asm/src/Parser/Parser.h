#pragma once

#include <memory>
#include "Files/InputFile.h"
#include "Interface_Exception.h"

namespace V2MPAsm
{
	class Parser
	{
	public:
		Parser(const std::shared_ptr<InputFile>& inputFile);

		ExceptionList ParseFile();

	private:
		std::shared_ptr<InputFile> m_InputFile;
	};
}
