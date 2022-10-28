#include "Parser/Parser.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Files/InputReader.h"
#include "Interface_Exception.h"
#include "Parser/Tokeniser.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	// Make this configurable?
	static constexpr size_t MAX_ALLOWED_ERRORS = 20;

	Parser::Parser(const std::shared_ptr<InputFile>& inputFile) :
		m_InputFile(inputFile)
	{
	}

	ExceptionList Parser::ParseFile()
	{
		InputReader reader(m_InputFile);
		ExceptionList outList;
		size_t recordedErrors = 0;

		m_State = State::DEFAULT;

		while ( m_State != State::TERMINATED )
		{
			// It may be advantageous from an error reporting point of view to actually
			// perform an iteration if the reader is at EOF but the state machine has
			// not yet termianted. However, only allow this to be done once, just as
			// a sanity check against infinite loops.
			const bool readerWasEOF = reader.IsEOF();

			try
			{
				m_State = ProcessToken(Tokeniser().EmitToken(reader));
			}
			catch ( const AssemblerException& ex )
			{
				outList.emplace_back(CreateException(ex.GetPublicException()));

				if ( ex.GetPublicException().GetType() == V2MPASM_EXCEPTION_ERROR )
				{
					++recordedErrors;
				}
			}

			if ( recordedErrors >= MAX_ALLOWED_ERRORS )
			{
				outList.emplace_back(
					CreateErrorException(
						PublicErrorID::EXCEEDED_ERROR_COUNT,
						reader.GetPath(),
						reader.GetCurrentLine(),
						reader.GetCurrentColumn(),
						"Encountered " + std::to_string(recordedErrors) + " errors, terminating parsing."
					)
				);

				m_State = State::TERMINATED;
			}
			else if ( readerWasEOF && m_State != State::TERMINATED )
			{
				outList.emplace_back(
					CreateInternalWarningException(
						reader.GetPath(),
						reader.GetCurrentLine(),
						reader.GetCurrentColumn(),
						"Parser did not terminate correctly at end of file."
					)
				);

				m_State = State::TERMINATED;
			}
		}

		return outList;
	}

	Parser::State Parser::ProcessToken(const Tokeniser::Token& /* token */)
	{
		// TODO
		return State::TERMINATED;
	}
}
