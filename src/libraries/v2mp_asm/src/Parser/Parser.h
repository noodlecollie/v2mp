#pragma once

#include <exception>
#include <memory>
#include <optional>
#include <cstdint>
#include "Files/InputFile.h"
#include "Interface_Exception.h"
#include "Exceptions/AssemblerException.h"
#include "Parser/Tokeniser.h"
#include "ProgramModel/ProgramBuilder.h"
#include "ProgramModel/ProgramModel.h"

namespace V2MPAsm
{
	class InputReader;

	class Parser
	{
	public:
		struct ParseResult
		{
			std::unique_ptr<ProgramModel> programModel;
			ExceptionList exceptions;

			ParseResult() = default;

			// Move only.
			ParseResult(const ParseResult& other) = delete;
			ParseResult(ParseResult&& other) = default;
			ParseResult& operator =(const ParseResult& other) = delete;
			ParseResult& operator =(ParseResult&& other) = default;
		};

		ParseResult ParseFile(const std::shared_ptr<InputFile>& inputFile) noexcept;

	private:
#define PARSER_STATE_LIST \
	LIST_ITEM(BEGIN_LINE = 0, "BeginLine") \
	LIST_ITEM(SKIP_LINE, "SkipLine") \
	LIST_ITEM(BUILD_CODE_WORD, "BuildCodeWord") \
	LIST_ITEM(END_OF_FILE, "EndOfFile") \
	LIST_ITEM(TERMINATED, "Terminated")

		enum class State
		{
#define LIST_ITEM(name, desc) name,
			PARSER_STATE_LIST
#undef LIST_ITEM
		};

		class ParserException : public std::exception
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
				PublicErrorID errorID,
				size_t line,
				size_t column,
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
				InputReader& reader,
				PublicWarningID errorID,
				size_t line,
				size_t column,
				const std::string& message,
				const std::optional<State>& inNextState = std::optional<State>()
			);

			ParserException(
				const AssemblerException& ex,
				const std::optional<State>& inNextState = std::optional<State>()
			);

			ParserException(const std::optional<State>& inNextState = std::optional<State>());

			ParserException& operator <<(const AssemblerException& exception);
			ParserException& operator <<(AssemblerException&& exception);

			std::optional<State> nextState;
			std::vector<AssemblerException> exceptionList;
		};

		struct ParserData
		{
			std::shared_ptr<InputFile> inputfile;
			State state = State::BEGIN_LINE;
			ExceptionList exceptionList;
			size_t recordedErrors = 0;
			ProgramBuilder programBuilder;

			void AppendException(const AssemblerException& ex)
			{
				exceptionList.emplace_back(CreateException(ex.GetPublicException()));

				if ( ex.GetPublicException().GetType() == V2MPASM_EXCEPTION_ERROR )
				{
					++recordedErrors;
				}
			}
		};

		static const char* GetStateName(State state);

		void InitialiseLocalData(const std::shared_ptr<InputFile>& inputFile);
		void ParseFileInternal();
		void PerformPostProcessing();
		void AdvanceState(InputReader& reader);
		State ProcessInputAndChooseNextState(InputReader& reader);

		State ProcessInput_BeginLine(InputReader& reader);
		State ProcessInput_SkipLine(InputReader& reader);
		State ProcessInput_BuildCodeWord(InputReader& reader);
		State ProcessInput_EndOfFile(InputReader& reader);

		State ProcessInput_CreateInstructionCodeWord(InputReader& reader, const Tokeniser::Token& token);
		State ProcessInput_CreateLabel(InputReader& reader, const Tokeniser::Token& token);
		State ProcessInput_AddArgumentToCodeWord(InputReader& reader, const Tokeniser::Token& token);
		State ProcessInput_ParseAndAddLabelRefToCodeWord(InputReader& reader, const Tokeniser::Token& token);
		State ProcessInput_ValidateAndCommitCodeWord(InputReader& reader, Tokeniser::TokenType tokenType);

		void ResolveAllLabelReferences();
		void ResolveLabelReference(const CodeWord& codeWord, CodeWordArg& arg);
		void FullyValidateAllCodeWords();

		Tokeniser::Token GetNextToken(
			InputReader& reader,
			uint32_t allowedTokens = ~0,
			const std::optional<State>& failState = std::optional<State>()
		) const;

		std::unique_ptr<ParserData> m_Data;
	};
}
