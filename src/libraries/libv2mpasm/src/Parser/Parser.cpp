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
#include "LibBaseUtil/Array.h"
#include "Parser/Tokeniser.h"
#include "ProgramModel/InstructionMeta.h"
#include "Utils/StringUtils.h"
#include "ProgramModel/CodeWordValidation.h"
#include "ProgramModel/ValidationUtils.h"

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
		if ( !inputFile )
		{
			ParseResult result;

			result.exceptions.emplace_back(
				CreateErrorException(
					PublicErrorID::INTERNAL,
					std::string(),
					LINE_NUMBER_BASE,
					COLUMN_NUMBER_BASE,
					"Input file was not valid."
				)
			);

			return result;
		}

		return RunParseSequence(std::make_unique<ParserData>(inputFile));
	}

	Parser::ParseResult Parser::ParseData(const std::string& inputPath, std::vector<char>&& inputData) noexcept
	{
		return RunParseSequence(std::make_unique<ParserData>(inputPath, std::move(inputData)));
	}

	Parser::ParseResult Parser::RunParseSequence(std::unique_ptr<ParserData> data)
	{
		ParseResult result;

		// No exceptions should make it this far up if everything is being used correctly,
		// but we wrap the calls just in case.
		try
		{
			if ( !data )
			{
				throw AssemblerException(
					PublicErrorID::INTERNAL,
					std::string(),
					LINE_NUMBER_BASE,
					COLUMN_NUMBER_BASE,
					"Invalid parser data was provided."
				);
			}

			m_Data = std::move(data);

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

		// Should always return via this path.
		m_Data.reset();
		return result;
	}

	void Parser::ParseFileInternal()
	{
		while ( m_Data->state != State::TERMINATED )
		{
			// We have an EOF parse state for tidying up once the end of the file has been reached.
			// However, we should only allow this state to be processed once, just as a sanity check
			// against infinite loops. readerWasEOF is checked later to make sure the parser
			// terminates properly.
			const bool readerWasEOF = m_Data->inputReader.IsEOF();

			try
			{
				AdvanceState();
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
						m_Data->inputReader.GetPath(),
						m_Data->inputReader.GetCurrentLine(),
						m_Data->inputReader.GetCurrentColumn(),
						"Encountered " + std::to_string(m_Data->recordedErrors) + " errors, terminating parsing."
					)
				);

				m_Data->state = State::TERMINATED;
			}
			else if ( readerWasEOF && m_Data->state != State::TERMINATED )
			{
				m_Data->exceptionList.emplace_back(
					CreateInternalWarningException(
						m_Data->inputReader.GetPath(),
						m_Data->inputReader.GetCurrentLine(),
						m_Data->inputReader.GetCurrentColumn(),
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

	void Parser::AdvanceState()
	{
		try
		{
			m_Data->state = ProcessInputAndChooseNextState();
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
				m_Data->inputReader.GetPath(),
				m_Data->inputReader.GetCurrentLine(),
				m_Data->inputReader.GetCurrentColumn(),
				"An unhandled exception occurred during parsing."
			);
		}
	}

	Parser::State Parser::ProcessInputAndChooseNextState()
	{
		switch ( m_Data->state )
		{
			case State::BEGIN_LINE:
			{
				return ProcessInput_BeginLine();
			}

			case State::SKIP_LINE:
			{
				return ProcessInput_SkipLine();
			}

			case State::BUILD_CODE_WORD:
			{
				return ProcessInput_BuildCodeWord();
			}

			case State::END_OF_FILE:
			{
				return ProcessInput_EndOfFile();
			}

			default:
			{
				throw ParserException(
					m_Data->inputReader,
					PublicErrorID::INTERNAL,
					"Encountered unhandled parser state \"" + std::string(GetStateName(m_Data->state)) + "\".",
					State::TERMINATED
				);
			}
		}
	}

	Parser::State Parser::ProcessInput_BeginLine()
	{
		constexpr uint32_t ALLOWED_TOKENS =
			TokenType::EndOfFile |		// Reached end of file
			TokenType::EndOfLine |		// Empty line
			TokenType::AlnumString |	// Instruction name
			TokenType::Label			// Label definition
			;

		const Tokeniser::Token token = GetNextToken(ALLOWED_TOKENS);

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
			return ProcessInput_CreateInstructionCodeWord(token);
		}

		// Otherwise, token must be a label definition.
		return ProcessInput_CreateLabel(token);
	}

	Parser::State Parser::ProcessInput_SkipLine()
	{
		while ( true )
		{
			const Tokeniser::Token token = GetNextToken(static_cast<uint32_t>(~0), State::TERMINATED);

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

	Parser::State Parser::ProcessInput_BuildCodeWord()
	{
		constexpr uint32_t ALLOWED_TOKENS =
			TokenType::NumericLiteral |				// Literal value
			TokenType::HighSelector |				// High section of a label
			TokenType::LowSelector |				// Low section of a label
			TokenType::SignedDistanceSelector |		// Distance to a label
			TokenType::UnsignedDistanceSelector |	// Distance to a label
			TokenType::EndOfFile |					// Ends instruction
			TokenType::EndOfLine					// Ends instruction
			;

		const Tokeniser::Token token = GetNextToken(ALLOWED_TOKENS, State::SKIP_LINE);

		if ( token.type == TokenType::EndOfFile || token.type == TokenType::EndOfLine )
		{
			return ProcessInput_ValidateAndCommitCodeWord(token.type);
		}

		return ProcessInput_AddArgumentToCodeWord(token);
	}

	Parser::State Parser::ProcessInput_EndOfFile()
	{
		if ( m_Data->programBuilder.HasEnqueuedLabels() )
		{
			ParserException ex(State::TERMINATED);

			for ( const std::string& labelName : m_Data->programBuilder.EnqueuedLabels() )
			{
				ex << AssemblerException(
					PublicWarningID::LABEL_DISCARDED,
					m_Data->inputReader.GetPath(),
					m_Data->inputReader.GetCurrentLine(),
					m_Data->inputReader.GetCurrentColumn(),
					"Redundant label \"" + labelName + "\" present at end of file will be discarded."
				);
			}

			throw ex;
		}

		return State::TERMINATED;
	}

	Parser::State Parser::ProcessInput_CreateInstructionCodeWord(const Tokeniser::Token& token)
	{
		const InstructionMeta* instructionMeta = GetInstructionMeta(token.token);

		if ( !instructionMeta )
		{
			throw ParserException(
				m_Data->inputReader,
				PublicErrorID::UNRECOGNISED_INSTRUCTION,
				"Unrecognised instruction: " + token.token,
				State::SKIP_LINE
			);
		}

		m_Data->programBuilder.PrepareCodeWord(token.line, token.column, instructionMeta->type);
		return State::BUILD_CODE_WORD;
	}

	Parser::State Parser::ProcessInput_CreateLabel(const Tokeniser::Token& token)
	{
		if ( m_Data->programBuilder.HasLabel(token.token) )
		{
			const std::optional<uint16_t> address = m_Data->programBuilder.GetLabelAddress(token.token);

			if ( address.has_value() )
			{
				throw ParserException(
					m_Data->inputReader,
					PublicErrorID::DUPLICATE_LABEL,
					"Label with name \"" + token.token + "\" already exists.",
					State::SKIP_LINE
				);
			}
			else
			{
				throw ParserException(
					m_Data->inputReader,
					PublicWarningID::REDUNDANT_LABEL,
					"Label \"" + token.token + "\" specified more than once.",
					State::SKIP_LINE
				);
			}
		}

		m_Data->programBuilder.EnqueueNextLabel(token.token);

		const Tokeniser::Token nextToken = GetNextToken(TokenType::EndOfLine | TokenType::EndOfFile, State::SKIP_LINE);
		const State nextState = nextToken.type == TokenType::EndOfFile ? State::END_OF_FILE : State::BEGIN_LINE;

		return nextState;
	}

	Parser::State Parser::ProcessInput_AddArgumentToCodeWord(const Tokeniser::Token& token)
	{
		switch ( token.type )
		{
			case TokenType::NumericLiteral:
			{
				int32_t value = 0;

				try
				{
					value = ParseInteger(token.token);
				}
				catch ( const std::exception& )
				{
					throw ParserException(
						m_Data->inputReader,
						PublicErrorID::UNIMPLEMENTED,
						"Invalid numeric literal: " + token.token
					);
				}

				m_Data->programBuilder.GetCurrentCodeWord().AddArgument(token.column, value);
				return State::BUILD_CODE_WORD;
			}

			case TokenType::HighSelector:
			case TokenType::LowSelector:
			case TokenType::SignedDistanceSelector:
			case TokenType::UnsignedDistanceSelector:
			{
				return ProcessInput_ParseAndAddLabelRefToCodeWord(token);
			}

			default:
			{
				throw ParserException(
					m_Data->inputReader,
					PublicErrorID::INTERNAL,
					"Unexpected token when attempting to add code word argument.",
					State::TERMINATED
				);
			}
		}
	}

	Parser::State Parser::ProcessInput_ParseAndAddLabelRefToCodeWord(const Tokeniser::Token& token)
	{
		const Tokeniser::Token labelString = GetNextToken(TokenType::Label, State::SKIP_LINE);

		switch ( token.type )
		{
			case TokenType::HighSelector:
			{
				m_Data->programBuilder.GetCurrentCodeWord().AddArgument(
					token.column,
					LabelReference::ReferenceType::UPPER_BYTE_OF_ADDRESS,
					labelString.token
				);

				break;
			}

			case TokenType::LowSelector:
			{
				m_Data->programBuilder.GetCurrentCodeWord().AddArgument(
					token.column,
					LabelReference::ReferenceType::LOWER_BYTE_OF_ADDRESS,
					labelString.token
				);

				break;
			}

			case TokenType::SignedDistanceSelector:
			{
				m_Data->programBuilder.GetCurrentCodeWord().AddArgument(
					token.column,
					LabelReference::ReferenceType::SIGNED_NUM_WORDS_DIST_TO_LABEL,
					labelString.token
				);

				break;
			}

			case TokenType::UnsignedDistanceSelector:
			{
				m_Data->programBuilder.GetCurrentCodeWord().AddArgument(
					token.column,
					LabelReference::ReferenceType::UNSIGNED_NUM_WORDS_DIST_TO_LABEL,
					labelString.token
				);

				break;
			}

			default:
			{
				throw ParserException(
					m_Data->inputReader,
					PublicErrorID::INTERNAL,
					"Unexpected token when attempting to parse label ref.",
					State::TERMINATED
				);
			}
		}

		return State::BUILD_CODE_WORD;
	}

	Parser::State Parser::ProcessInput_ValidateAndCommitCodeWord(Tokeniser::TokenType tokenType)
	{
		const Parser::State nextState = tokenType == Tokeniser::TokenType::EndOfFile
			? Parser::State::TERMINATED
			: Parser::State::BEGIN_LINE;

		std::shared_ptr<CodeWord> currentCodeWord = m_Data->programBuilder.GetCurrentCodeWordPtr();

		// Don't validate label refs here, as the labels may not yet be resolved.
		std::vector<ValidationFailure> validationFailures = ValidateCodeWord(currentCodeWord, false);

		// Submit the current code word, regardless of whether it's valid or not.
		// This means that one invalid code word won't terminate parsing, and should
		// therefore allow us to continue to generate potential errors from subsequent
		// lines. If any errors are produced, the parser will stop once the first
		// parsing phase is finished, and will not go on to produce output.
		m_Data->programBuilder.SubmitCurrentCodeWord();

		if ( !validationFailures.empty() )
		{
			ParserException ex(nextState);

			for ( const ValidationFailure& failure : validationFailures )
			{
				ex << ToAssemblerException(failure, m_Data->inputReader.GetPath(), *currentCodeWord);
			}

			throw ex;
		}

		return nextState;
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
				// Should never happen, so just throw something basic.
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
					m_Data->inputReader.GetPath(),
					codeWord.GetLine(),
					arg.GetColumn(),
					"Label with name \"" + labelName + "\" does not exist."
				)
			);

			return;
		}

		arg.SetValue(model.GetValueOfLabelReference(codeWord, *targetCodeWord, labelRef.GetReferenceType()));
	}

	void Parser::FullyValidateAllCodeWords()
	{
		const ProgramModel& model = m_Data->programBuilder.GetProgramModel();
		const size_t codeWordCount = model.GetCodeWordCount();

		for ( size_t index = 0; index < codeWordCount; ++index )
		{
			std::shared_ptr<CodeWord> codeWord = model.GetCodeWord(index);
			ValidateCodeWord(codeWord, true);
		}
	}

	Tokeniser::Token Parser::GetNextToken(uint32_t allowedTokens, const std::optional<State>& failState) const
	{
		try
		{
			const Tokeniser::Token token = Tokeniser().EmitToken(m_Data->inputReader);

			if ( !(token.type & allowedTokens) )
			{
				throw ParserException(
					m_Data->inputReader,
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
		return index < BaseUtil::ArraySize(STATE_STRINGS) ? STATE_STRINGS[index] : "Unknown";
	}
}
