#include <exception>
#include <memory>
#include <string>
#include "Parser/Parser.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Files/InputReader.h"
#include "Interface_Exception.h"
#include "Parser/Tokeniser.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/ProgramModel.h"
#include "Utils/ParsingUtils.h"
#include "Utils/ArrayUtils.h"
#include "Parser/Tokeniser.h"
#include "ProgramModel/InstructionMeta.h"
#include "Utils/StringUtils.h"
#include "ProgramModel/CodeWordValidation.h"

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
		std::exception(),
		nextState(inNextState)
	{
		(*this) << AssemblerException(errorID, reader.GetPath(), reader.GetCurrentLine(), reader.GetCurrentColumn(), message);
	}

	Parser::ParserException::ParserException(
		InputReader& reader,
		PublicErrorID errorID,
		size_t line,
		size_t column,
		const std::string& message,
		const std::optional<State>& inNextState
	) :
		std::exception(),
		nextState(inNextState)
	{
		(*this) << AssemblerException(errorID, reader.GetPath(), line, column, message);
	}

	Parser::ParserException::ParserException(
		InputReader& reader,
		PublicWarningID errorID,
		const std::string& message,
		const std::optional<State>& inNextState
	) :
		std::exception(),
		nextState(inNextState)
	{
		(*this) << AssemblerException(errorID, reader.GetPath(), reader.GetCurrentLine(), reader.GetCurrentColumn(), message);
	}

	Parser::ParserException::ParserException(
		InputReader& reader,
		PublicWarningID errorID,
		size_t line,
		size_t column,
		const std::string& message,
		const std::optional<State>& inNextState
	) :
		std::exception(),
		nextState(inNextState)
	{
		(*this) << AssemblerException(errorID, reader.GetPath(), line, column, message);
	}

	Parser::ParserException::ParserException(
		const AssemblerException& ex,
		const std::optional<State>& inNextState
	) :
		std::exception(),
		nextState(inNextState)
	{
		(*this) << ex;
	}

	Parser::ParserException::ParserException(const std::optional<State>& inNextState) :
		std::exception(),
		nextState(inNextState)
	{
	}

	Parser::ParserException& Parser::ParserException::operator <<(const AssemblerException& exception)
	{
		exceptionList.emplace_back(exception);
		return *this;
	}

	Parser::ParserException& Parser::ParserException::operator <<(AssemblerException&& exception)
	{
		exceptionList.emplace_back(std::move(exception));
		return *this;
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
			PerformPostProcessing();

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
			// We have an EOF parse state for tidying up once the end of the file has been reached.
			// However, we should only allow this state to be processed once, just as a sanity check
			// against infinite loops. readerWasEOF is checked later to make sure the parser
			// terminates properly.
			const bool readerWasEOF = reader.IsEOF();

			try
			{
				AdvanceState(reader);
			}
			catch ( const AssemblerException& ex )
			{
				m_Data->AppendException(ex);
			}
			catch ( const ParserException& ex )
			{
				for ( const AssemblerException& assemblerEx : ex.exceptionList )
				{
					m_Data->AppendException(assemblerEx);
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

	void Parser::PerformPostProcessing()
	{
		if ( m_Data->recordedErrors > 0 )
		{
			// Parsing failed, so don't bother with any of this.
			return;
		}

		ResolveAllLabelReferences();
		FullyValidateAllCodeWords();
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
			}

			case State::SKIP_LINE:
			{
				return ProcessInput_SkipLine(reader);
			}

			case State::BUILD_CODE_WORD:
			{
				return ProcessInput_BuildCodeWord(reader);
			}

			case State::END_OF_FILE:
			{
				return ProcessInput_EndOfFile(reader);
			}

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
			return State::END_OF_FILE;
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

	Parser::State Parser::ProcessInput_SkipLine(InputReader& reader)
	{
		while ( true )
		{
			const Tokeniser::Token token = GetNextToken(reader, static_cast<uint32_t>(~0), State::TERMINATED);

			if ( token.type == TokenType::EndOfFile )
			{
				return State::END_OF_FILE;
			}

			if ( token.type == TokenType::EndOfLine )
			{
				return State::BEGIN_LINE;
			}
		}
	}

	Parser::State Parser::ProcessInput_BuildCodeWord(InputReader& reader)
	{
		constexpr uint32_t ALLOWED_ARG_TOKENS =
			TokenType::NumericLiteral |		// Literal value
			TokenType::HighSelector |		// High section of a label
			TokenType::LowSelector |		// Low section of a label
			TokenType::DistanceSelector		// Distance to a label
			;

		constexpr uint32_t ALLOWED_INSTRUCTION_TERMINATOR_TOKENS =
			TokenType::EndOfFile |
			TokenType::EndOfLine
			;

		constexpr uint32_t ALL_ALLOWED_TOKENS = ALLOWED_ARG_TOKENS | ALLOWED_INSTRUCTION_TERMINATOR_TOKENS;

		CodeWord& codeWord = m_Data->programBuilder.GetCurrentCodeWord();
		const InstructionMeta& instructionMeta = GetInstructionMeta(codeWord.GetInstructionType());
		const bool moreArgumentsRequired = codeWord.GetArgumentCount() < instructionMeta.args.size();

		const uint32_t allowedTokens = moreArgumentsRequired
			? ALLOWED_ARG_TOKENS
			: ALLOWED_INSTRUCTION_TERMINATOR_TOKENS;

		// Get any possible token, and then customise any exception based on the state we're in.
		const Tokeniser::Token token = GetNextToken(reader, ALL_ALLOWED_TOKENS, State::SKIP_LINE);

		if ( !(token.type & allowedTokens) )
		{
			if ( moreArgumentsRequired )
			{
				throw ParserException(
					reader,
					PublicErrorID::UNEXPECTED_TOKEN,
					"Expected number or label reference, but got " + Tokeniser::TokenPrintableString(token),
					State::SKIP_LINE
				);
			}
			else
			{
				throw ParserException(
					reader,
					PublicErrorID::UNEXPECTED_TOKEN,
					instructionMeta.key + " instruction expects " + std::to_string(instructionMeta.args.size()) + "arguments.",
					State::SKIP_LINE
				);
			}
		}

		if ( token.type == TokenType::EndOfFile || token.type == TokenType::EndOfLine )
		{
			return ProcessInput_ValidateAndCommitCodeWord(reader, token.type);
		}

		return ProcessInput_AddArgumentToCodeWord(reader, token);
	}

	Parser::State Parser::ProcessInput_EndOfFile(InputReader& reader)
	{
		const std::string nextLabel = m_Data->programBuilder.GetNextLabelName();

		if ( !nextLabel.empty() )
		{
			throw ParserException(
				reader,
				PublicWarningID::REDUNDANT_LABEL,
				"Redundant label \"" + nextLabel + "\" present at end of file will be discarded.",
				State::TERMINATED
			);
		}

		return State::TERMINATED;
	}

	Parser::State Parser::ProcessInput_CreateInstructionCodeWord(InputReader& reader, const Tokeniser::Token& token)
	{
		const InstructionMeta* instructionMeta = GetInstructionMeta(token.token);

		if ( !instructionMeta )
		{
			throw ParserException(
				reader,
				PublicErrorID::UNRECOGNISED_INSTRUCTION,
				"Unrecognised instruction: " + token.token,
				State::SKIP_LINE
			);
		}

		m_Data->programBuilder.PrepareCodeWord(token.line, token.column, instructionMeta->type);
		return State::BUILD_CODE_WORD;
	}

	Parser::State Parser::ProcessInput_CreateLabel(InputReader& reader, const Tokeniser::Token& token)
	{
		if ( m_Data->programBuilder.HasLabel(token.token) )
		{
			const std::optional<uint16_t> address = m_Data->programBuilder.GetLabelAddress(token.token);

			if ( address.has_value() )
			{
				throw ParserException(
					reader,
					PublicErrorID::DUPLICATE_LABEL,
					"Label with name \"" + token.token + "\" already exists.",
					State::SKIP_LINE
				);
			}
			else
			{
				throw ParserException(
					reader,
					PublicWarningID::REDUNDANT_LABEL,
					"Label \"" + token.token + "\" specified more than once.",
					State::SKIP_LINE
				);
			}
		}

		const std::string existingLabel = m_Data->programBuilder.GetNextLabelName();
		m_Data->programBuilder.SetNextLabelName(token.token);

		const Tokeniser::Token nextToken = GetNextToken(reader, TokenType::EndOfLine | TokenType::EndOfFile, State::SKIP_LINE);
		const State nextState = nextToken.type == TokenType::EndOfFile ? State::END_OF_FILE : State::BEGIN_LINE;

		if ( !existingLabel.empty() )
		{
			// We overwrote a label that wasn't submitted yet.
			throw ParserException(
				reader,
				PublicWarningID::LABEL_DISCARDED,
				"Label \"" + token.token + "\" will cause previous label \"" + existingLabel + "\" to be discarded.",
				nextState
			);
		}

		return nextState;
	}

	Parser::State Parser::ProcessInput_AddArgumentToCodeWord(InputReader& reader, const Tokeniser::Token& token)
	{
		if ( token.type == TokenType::NumericLiteral )
		{
			int32_t value = 0;

			try
			{
				value = ParseInteger(token.token);
			}
			catch ( const std::exception& )
			{
				throw ParserException(
					reader,
					PublicErrorID::UNIMPLEMENTED,
					"Invalid numeric literal: " + token.token
				);
			}

			// Arguments must be validated once code word is complete,
			// as the validity of one argument may depend on the value of another.
			m_Data->programBuilder.GetCurrentCodeWord().AddArgument(token.column, value);
		}
		else
		{
			// TODO: Deal with label reference
			throw ParserException(
				reader,
				PublicErrorID::UNIMPLEMENTED,
				"Label ref instruction argument parsing is not yet implemented.",
				State::TERMINATED
			);
		}

		return State::BUILD_CODE_WORD;
	}

	Parser::State Parser::ProcessInput_ValidateAndCommitCodeWord(InputReader& reader, Tokeniser::TokenType /* tokenType */)
	{
		CodeWord& currentCodeWord = m_Data->programBuilder.GetCurrentCodeWord();
		std::vector<ValidationFailure> validationFailures = ValidateCodeWord(currentCodeWord, false);

		if ( !validationFailures.empty() )
		{
			ParserException ex(Parser::State::BUILD_CODE_WORD);

			for ( const ValidationFailure& failure : validationFailures )
			{
				if ( failure.IsError() )
				{
					ex.nextState = Parser::State::TERMINATED;
				}

				ex << ToAssemblerException(failure, reader.GetPath(), currentCodeWord);
			}

			throw ex;
		}

		// TODO: Implement
		throw ParserException(
			reader,
			PublicErrorID::UNIMPLEMENTED,
			"Committing code word is not yet implemented.",
			State::TERMINATED
		);
	}

	void Parser::ResolveAllLabelReferences()
	{
		ProgramBuilder& builder = m_Data->programBuilder;
		const ProgramModel& model = builder.GetProgramModel();
		const size_t numCodewords = model.GetCodeWordCount();

		for ( size_t index = 0; index < numCodewords; ++index )
		{
			std::shared_ptr<CodeWord> codeWord = model.GetCodeWord(index);

			if ( !codeWord )
			{
				// Should never happen, and we don't have reader information any more here,
				// so just throw something basic.
				throw std::runtime_error("ResolveAllLabelReferences encountered invalid codeword");
			}

			const size_t numArgs = codeWord->GetArgumentCount();

			for ( size_t argIndex = 0; argIndex < numArgs; ++argIndex )
			{
				CodeWordArg* arg = codeWord->GetArgument(argIndex);

				if ( arg->IsLabelReference() )
				{
					ResolveLabelReference(*codeWord, *arg);
				}
			}
		}
	}

	void Parser::ResolveLabelReference(const CodeWord& codeWord, CodeWordArg& arg)
	{
		const LabelReference labelRef = arg.GetLabelReference();
		const std::string labelName = labelRef.GetLabelName();
		const ProgramModel& model = m_Data->programBuilder.GetProgramModel();
		std::shared_ptr<CodeWord> targetCodeWord = model.CodeWordForLabel(labelName);

		if ( !targetCodeWord )
		{
			// TODO: This does not keep track of max allowed errors!
			m_Data->exceptionList.emplace_back(
				CreateErrorException(
					PublicErrorID::INVALID_LABEL_REFERENCE,
					m_Data->inputfile->GetPath(),
					codeWord.GetLine(),
					arg.GetColumn(),
					"Label with name \"" + labelName + "\" does not exist."
				)
			);

			return;
		}

		const uint16_t value = model.GetValueOfLabelReference(codeWord, *targetCodeWord, labelRef.GetReferenceType());
		arg.SetValue(static_cast<int32_t>(value));
	}

	void Parser::FullyValidateAllCodeWords()
	{
		// TODO
		throw AssemblerException(
			PublicErrorID::UNIMPLEMENTED,
			m_Data->inputfile->GetPath(),
			0,
			0,
			"FullyValidateAllCodeWords not yet implemented"
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
					"Encountered unexpected " + Tokeniser::TokenPrintableString(token),
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
