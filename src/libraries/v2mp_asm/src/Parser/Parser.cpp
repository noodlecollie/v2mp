#include "Parser/Parser.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Files/InputReader.h"
#include "Interface_Exception.h"
#include "Parser/Tokeniser.h"
#include "ProgramModel/ProgramModel.h"
#include "Utils/ParsingUtils.h"
#include "Utils/ArrayUtils.h"
#include "Parser/Tokeniser.h"
#include <memory>

namespace V2MPAsm
{
	using TokenType = Tokeniser::TokenType;

	// Make this configurable?
	static constexpr size_t MAX_ALLOWED_ERRORS = 20;

	Parser::ParserException::ParserException(
		InputReader& reader,
		PublicErrorID errorID,
		const std::string& message,
		const std::optional<State>& inNextState
	) :
		AssemblerException(errorID, reader.GetPath(), reader.GetCurrentLine(), reader.GetCurrentColumn(), message),
		nextState(inNextState)
	{
	}

	Parser::ParserException::ParserException(
		InputReader& reader,
		PublicWarningID errorID,
		const std::string& message,
		const std::optional<State>& inNextState
	) :
		AssemblerException(errorID, reader.GetPath(), reader.GetCurrentLine(), reader.GetCurrentColumn(), message),
		nextState(inNextState)
	{
	}

	Parser::ParserException::ParserException(
		const AssemblerException& ex,
		const std::optional<State>& inNextState
	) :
		AssemblerException(ex),
		nextState(inNextState)
	{
	}

	Parser::ParseResult Parser::ParseFile(const std::shared_ptr<InputFile>& inputFile) noexcept
	{
		ParseResult result;

		// No exceptions should make it this far up if everything is being used correctly,
		// but we wrap the calls just in case.
		try
		{
			InitialiseLocalData(inputFile);
			ParseFileInternal();

			result.exceptions = m_Data->exceptionList;

			if ( m_Data->recordedErrors < 1 )
			{
				result.programModel = m_Data->programBuilder.TakeProgramModel();
			}
		}
		catch ( const AssemblerException& ex )
		{
			result.programModel.reset();
			result.exceptions.emplace_back(CreateException(ex.GetPublicException()));
		}
		catch ( ... )
		{
			result.programModel.reset();
			result.exceptions.emplace_back(
				CreateErrorException(
					PublicErrorID::INTERNAL,
					std::string(),
					LINE_NUMBER_BASE,
					COLUMN_NUMBER_BASE,
					"An unhandled exception occurred while executing ParseFile()."
				)
			);
		}

		m_Data.reset();
		return result;
	}

	void Parser::InitialiseLocalData(const std::shared_ptr<InputFile>& inputFile)
	{
		if ( !inputFile )
		{
			throw AssemblerException(
				PublicErrorID::INTERNAL,
				std::string(),
				LINE_NUMBER_BASE,
				COLUMN_NUMBER_BASE,
				"Input file was not valid."
			);
		}

		m_Data = std::make_unique<ParserData>();

		m_Data->inputfile = inputFile;
	}

	void Parser::ParseFileInternal()
	{
		InputReader reader(m_Data->inputfile);

		while ( m_Data->state != State::TERMINATED )
		{
			// It may be advantageous from an error reporting point of view to actually
			// perform an iteration if the reader is at EOF but the state machine has
			// not yet termianted. However, only allow this to be done once, just as
			// a sanity check against infinite loops.
			const bool readerWasEOF = reader.IsEOF();

			try
			{
				AdvanceState(reader);
			}
			catch ( const AssemblerException& ex )
			{
				m_Data->exceptionList.emplace_back(CreateException(ex.GetPublicException()));

				if ( ex.GetPublicException().GetType() == V2MPASM_EXCEPTION_ERROR )
				{
					++m_Data->recordedErrors;
				}
			}

			if ( m_Data->recordedErrors >= MAX_ALLOWED_ERRORS )
			{
				m_Data->exceptionList.emplace_back(
					CreateErrorException(
						PublicErrorID::EXCEEDED_ERROR_COUNT,
						reader.GetPath(),
						reader.GetCurrentLine(),
						reader.GetCurrentColumn(),
						"Encountered " + std::to_string(m_Data->recordedErrors) + " errors, terminating parsing."
					)
				);

				m_Data->state = State::TERMINATED;
			}
			else if ( readerWasEOF && m_Data->state != State::TERMINATED )
			{
				m_Data->exceptionList.emplace_back(
					CreateInternalWarningException(
						reader.GetPath(),
						reader.GetCurrentLine(),
						reader.GetCurrentColumn(),
						"Parser did not terminate correctly at end of file."
					)
				);

				m_Data->state = State::TERMINATED;
			}
		}
	}

	void Parser::AdvanceState(InputReader& reader)
	{
		try
		{
			m_Data->state = ProcessInputAndChooseNextState(reader);
		}
		catch ( const ParserException& ex )
		{
			if ( ex.nextState )
			{
				m_Data->state = ex.nextState.value();
			}

			throw ex;
		}
		catch ( ... )
		{
			// This should never happen at this level - we always want to be throwing
			// ParserExceptions from ProcessInputAndChooseNextState(). If we don't,
			// the fail state to move to is not well-defined. For safety, terminate here.
			m_Data->state = State::TERMINATED;

			throw AssemblerException(
				PublicErrorID::INTERNAL,
				reader.GetPath(),
				reader.GetCurrentLine(),
				reader.GetCurrentColumn(),
				"An unhandled exception occurred during parsing."
			);
		}
	}

	Parser::State Parser::ProcessInputAndChooseNextState(InputReader& reader)
	{
		switch ( m_Data->state )
		{
			case State::BEGIN_LINE:
			{
				return ProcessInput_BeginLine(reader);
			};

			default:
			{
				throw ParserException(
					reader,
					PublicErrorID::INTERNAL,
					"Encountered unhandled parser state \"" + std::string(GetStateName(m_Data->state)) + "\".",
					State::TERMINATED
				);
			}
		}
	}

	Parser::State Parser::ProcessInput_BeginLine(InputReader& reader)
	{
		constexpr uint32_t ALLOWED_TOKENS =
			TokenType::EndOfFile |		// Reached end of file
			TokenType::EndOfLine |		// Empty line
			TokenType::AlnumString |	// Instruction name
			TokenType::Label			// Label definition
			;

		const Tokeniser::Token token = GetNextToken(reader, ALLOWED_TOKENS);

		if ( token.type == TokenType::EndOfFile )
		{
			return State::TERMINATED;
		}

		if ( token.type == TokenType::EndOfLine )
		{
			// Ignore empty line and prepare for the next line.
			return State::BEGIN_LINE;
		}

		if ( token.type == TokenType::AlnumString )
		{
			return ProcessInput_CreateInstructionCodeWord(reader, token);
		}

		// Otherwise, token must be a label definition.
		return ProcessInput_CreateLabel(reader, token);
	}

	Parser::State Parser::ProcessInput_CreateInstructionCodeWord(InputReader& reader, const Tokeniser::Token& /* token */)
	{
		// TODO: Handle creation of new instruction.
		throw ParserException(
			reader,
			PublicErrorID::UNIMPLEMENTED,
			"Creation of code word for instruction is not yet implemented.",
			State::TERMINATED
		);
	}

	Parser::State Parser::ProcessInput_CreateLabel(InputReader& reader, const Tokeniser::Token& /* token */)
	{
		// TODO: Implement
		throw ParserException(
			reader,
			PublicErrorID::UNIMPLEMENTED,
			"Creation of label is not yet implemented.",
			State::TERMINATED
		);
	}

	Tokeniser::Token Parser::GetNextToken(InputReader& reader, uint32_t allowedTokens, const std::optional<State>& failState) const
	{
		try
		{
			const Tokeniser::Token token = Tokeniser().EmitToken(reader);

			if ( !(token.type & allowedTokens) )
			{
				throw ParserException(
					reader,
					PublicErrorID::UNEXPECTED_TOKEN,
					"Encountered unexpected token of type \"" + Tokeniser::TokenName(token.type) + "\".",
					failState
				);
			}

			return token;
		}
		catch ( const AssemblerException& ex )
		{
			throw ParserException(ex, failState);
		}
	}

	const char* Parser::GetStateName(State state)
	{
		static const char* const STATE_STRINGS[] =
		{
#define LIST_ITEM(name, desc) desc,
			PARSER_STATE_LIST
#undef LIST_ITEM
		};

		const size_t index = static_cast<size_t>(state);
		return index < ArraySize(STATE_STRINGS) ? STATE_STRINGS[index] : "Unknown";
	}
}
