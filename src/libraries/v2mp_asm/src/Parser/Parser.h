#pragma once

#include <memory>
#include "Files/InputFile.h"
#include "Interface_Exception.h"
#include "Parser/Tokeniser.h"

namespace V2MPAsm
{
	class Parser
	{
	public:
		Parser(const std::shared_ptr<InputFile>& inputFile);

		ExceptionList ParseFile();

	private:
		enum class State
		{
			DEFAULT,
			TERMINATED
		};

		State ProcessToken(const Tokeniser::Token& token);

		std::shared_ptr<InputFile> m_InputFile;
		State m_State = State::DEFAULT;
	};
}
