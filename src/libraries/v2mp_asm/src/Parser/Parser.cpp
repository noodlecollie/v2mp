#include "Parser/Parser.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Interface_Exception.h"

namespace V2MPAsm
{
	Parser::Parser(const std::shared_ptr<InputFile>& inputFile) :
		m_InputFile(inputFile)
	{
	}

	ExceptionList Parser::ParseFile()
	{
		// TODO
		return { CreateUnimplementedException(m_InputFile->GetPath()) };
	}
}
