#pragma once

#include <memory>
#include <optional>
#include <cstdint>
#include "Files/InputFile.h"
#include "Interface_Exception.h"
#include "Exceptions/AssemblerException.h"
#include "Parser/Tokeniser.h"

namespace V2MPAsm
{
	class InputReader;

	class Parser
	{
	public:
		Parser(const std::shared_ptr<InputFile>& inputFile);

		ExceptionList ParseFile();

	private:
#define PARSER_STATE_LIST \
	LIST_ITEM(BEGIN_LINE = 0, "BeginLine") \
	LIST_ITEM(TERMINATED, "Terminated")

		enum class State
		{
#define LIST_ITEM(name, desc) name,
			PARSER_STATE_LIST
#undef LIST_ITEM
		};

		class ParserException : public AssemblerException
		{
		public:
			ParserException(
				InputReader& reader,
				PublicErrorID errorID,
				const std::string& message,
				const std::optional<State>& inNextState = std::optional<State>()
			);

			ParserException(
				InputReader& reader,
				PublicWarningID errorID,
				const std::string& message,
				const std::optional<State>& inNextState = std::optional<State>()
			);

			ParserException(
				const AssemblerException& ex,
				const std::optional<State>& inNextState = std::optional<State>()
			);

			std::optional<State> nextState;
		};

		static const char* GetStateName(State state);

		void AdvanceState(InputReader& reader);
		State ProcessInputAndChooseNextState(InputReader& reader);

		State ProcessInput_BeginLine(InputReader& reader);

		Tokeniser::Token GetNextToken(
			InputReader& reader,
			uint32_t allowedTokens = ~0,
			const std::optional<State>& failState = std::optional<State>()
		) const;

		std::shared_ptr<InputFile> m_InputFile;
		State m_State = State::BEGIN_LINE;
	};
}
