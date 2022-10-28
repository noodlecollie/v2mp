#include "Parser/Parser.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Files/InputReader.h"
#include "Interface_Exception.h"
#include "Parser/Tokeniser.h"
#include "Utils/ParsingUtils.h"

// REMOVE ME
#include <iostream>

namespace V2MPAsm
{
	Parser::Parser(const std::shared_ptr<InputFile>& inputFile) :
		m_InputFile(inputFile)
	{
	}

	ExceptionList Parser::ParseFile()
	{
		// TODO: Implement this properly.

		InputReader reader(m_InputFile);

		while ( true )
		{
			Tokeniser::Token token = Tokeniser().EmitToken(reader);

			std::cout
				<< "Token @ line " << token.line << ", column " << token.column << ": "
				<< Tokeniser::TokenName(token.type) << " \"" << token.token
				<< "\" (legnth " << token.token.size() << ")" << std::endl;

			if ( token.type == Tokeniser::TokenType::EndOfFile )
			{
				break;
			}
		}

		return { CreateUnimplementedException(m_InputFile->GetPath(), LINE_NUMBER_BASE, COLUMN_NUMBER_BASE, "File parsing not implemented yet.") };
	}
}
