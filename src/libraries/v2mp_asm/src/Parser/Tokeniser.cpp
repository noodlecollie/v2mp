#include <cctype>
#include <type_traits>
#include <utility>
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Parser/Tokeniser.h"
#include "Files/InputReader.h"
#include "Utils/ArrayUtils.h"

namespace V2MPAsm
{
	static constexpr const char* const SKIPPABLE_WHITESPACE = "\r\t\v\f ";
	static constexpr const char* const ANY_WHITESPACE = "\r\n\t\v\f ";
	static constexpr const char* const VALID_DIGITS_DEC = "0123456789";
	static constexpr const char* const VALID_DIGITS_HEX = "0123456789ABCDEFabcdef";
	static constexpr const char* const VALID_DIGITS_BIN = "01";

	static inline bool IsLetterNumberOrUnderscore(char ch)
	{
		return std::isalnum(ch) || ch == '_';
	}

	static inline bool IsLetterOrUnderscore(char ch)
	{
		return std::isalpha(ch) || ch == '_';
	}

	Tokeniser::TokeniserException::TokeniserException(
		InputReader& reader,
		PublicErrorID errorID,
		const std::string& message,
		SkipBehaviour inSkipBehaviour
	) :
		AssemblerException(errorID, reader.GetPath(), reader.GetCurrentLine(), reader.GetCurrentColumn(), message),
		skipBehaviour(inSkipBehaviour)
	{
	}

	Tokeniser::TokeniserException::TokeniserException(
		InputReader& reader,
		PublicWarningID errorID,
		const std::string& message,
		SkipBehaviour inSkipBehaviour
	) :
		AssemblerException(errorID, reader.GetPath(), reader.GetCurrentLine(), reader.GetCurrentColumn(), message),
		skipBehaviour(inSkipBehaviour)
	{
	}

	Tokeniser::TokeniserException::TokeniserException(
		InputReader& reader,
		PublicErrorID errorID,
		size_t line,
		size_t column,
		const std::string& message,
		SkipBehaviour inSkipBehaviour
	) :
		AssemblerException(errorID, reader.GetPath(), line, column, message),
		skipBehaviour(inSkipBehaviour)
	{
	}

	Tokeniser::TokeniserException::TokeniserException(
		InputReader& reader,
		PublicWarningID errorID,
		size_t line,
		size_t column,
		const std::string& message,
		SkipBehaviour inSkipBehaviour
	) :
		AssemblerException(errorID, reader.GetPath(), line, column, message),
		skipBehaviour(inSkipBehaviour)
	{
	}

	std::string Tokeniser::TokenName(TokenType tokenType)
	{
		const std::pair<TokenType, std::string> TOKEN_NAMES[] =
		{
#define LIST_ITEM(enumName, value, name, desc, printable) { TokenType::enumName, name },
			TOKEN_TYPE_LIST
#undef LIST_ITEM
		};

		for ( size_t index = 0; index < ArraySize(TOKEN_NAMES); ++index )
		{
			if ( TOKEN_NAMES[index].first == tokenType )
			{
				return TOKEN_NAMES[index].second;
			}
		}

		return "UnknownTokenType";
	}

	std::string Tokeniser::TokenReadableDescription(TokenType tokenType)
	{
		const std::pair<TokenType, std::string> TOKEN_DESCS[] =
		{
#define LIST_ITEM(enumName, value, name, desc, printable) { TokenType::enumName, desc },
			TOKEN_TYPE_LIST
#undef LIST_ITEM
		};

		for ( size_t index = 0; index < ArraySize(TOKEN_DESCS); ++index )
		{
			if ( TOKEN_DESCS[index].first == tokenType )
			{
				return TOKEN_DESCS[index].second;
			}
		}

		return "unknown";
	}

	bool Tokeniser::TokenIsPrintable(TokenType tokenType)
	{
		const std::pair<TokenType, bool> TOKEN_PRINTABLE[] =
		{
#define LIST_ITEM(enumName, value, name, desc, printable) { TokenType::enumName, printable },
			TOKEN_TYPE_LIST
#undef LIST_ITEM
		};

		for ( size_t index = 0; index < ArraySize(TOKEN_PRINTABLE); ++index )
		{
			if ( TOKEN_PRINTABLE[index].first == tokenType )
			{
				return TOKEN_PRINTABLE[index].second;
			}
		}

		return false;
	}

	std::string Tokeniser::TokenPrintableString(const Token& token)
	{
		return TokenIsPrintable(token.type)
			? TokenReadableDescription(token.type) + ": " + token.token
			: TokenReadableDescription(token.type);
	}

	Tokeniser::Token Tokeniser::EmitToken(InputReader& reader) const
	{
		try
		{
			return EmitTokenInternal(reader);
		}
		catch ( const TokeniserException& ex )
		{
			switch ( ex.skipBehaviour )
			{
				case TokeniserException::SkipBehaviour::UNTIL_WHITESPACE:
				{
					reader.SkipUntilAnyOf(ANY_WHITESPACE);
					break;
				}

				case TokeniserException::SkipBehaviour::ALNUM_OR_UNDERSCORE:
				{
					reader.SkipForAnyOf(&IsLetterNumberOrUnderscore);
					break;
				}

				default:
				{
					break;
				}
			}

			throw ex;
		}
	}

	Tokeniser::Token Tokeniser::EmitTokenInternal(InputReader& reader) const
	{
		while ( true )
		{
			// Ignore any preceding whitespace.
			reader.SkipForAnyOf(SKIPPABLE_WHITESPACE);

			// Take a snapshot to begin the token.
			size_t beginLine = reader.GetCurrentLine();
			size_t beginCol = reader.GetCurrentColumn();
			size_t beginPos = reader.GetCurrentPosition();
			char beginChar = reader.PeekChar();

			if ( beginChar == '\0' )
			{
				return Token(TokenType::EndOfFile, beginLine, beginCol);
			}

			if ( beginChar == '\n' )
			{
				reader.ReadChar();
				return Token(TokenType::EndOfLine, beginLine, beginCol);
			}

			if ( beginChar == '/' && reader.PeekChar(1) == '/' )
			{
				// Skip line comment and go around again.
				// We don't consume the newline here, so that it
				// may be detected next time.
				reader.SkipUntilAnyOf("\n");
				continue;
			}

			if ( beginChar == '/' && reader.PeekChar(1) == '*' )
			{
				// find the end of the comment.
				reader.SkipUntilAnyOf(
					[&reader](char ch) -> bool
					{
						return ch == '*' && reader.PeekChar(1) == '/';
					}
				);

				if ( reader.PeekChar(0) == '\0' || reader.PeekChar(1) == '\0' )
				{
					// Comment was unterminated.
					throw TokeniserException(reader, PublicWarningID::UNTERMINATED_TOKEN, "Unterminated block comment.");
				}

				// Consume the ending characters.
				reader.SkipChars(2);

				// Go around again.
				continue;
			}

			if ( beginChar == '#' )
			{
				// Preprocessor command.

				reader.ReadChar(); // Skip '#'
				reader.SkipUntilAnyOf("\n");

				size_t endPos = reader.GetCurrentPosition();
				reader.ReadChar(); // Skip '\n'

				return Token(
					TokenType::PreprocessorCommand,
					beginLine,
					beginCol,
					ExtractString(reader, beginPos, endPos)
				);
			}

			if ( beginChar == '+' || beginChar == '-' || (beginChar >= '0' && beginChar <= '9') )
			{
				// Numeric literal.
				return Token(TokenType::NumericLiteral, beginLine, beginCol, ExtractNumericLiteral(reader));
			}

			if ( beginChar == '<' )
			{
				// High byte selector.
				reader.ReadChar();

				return Token(
					TokenType::HighSelector,
					beginLine,
					beginCol,
					ExtractString(reader, beginPos, beginPos + 1)
				);
			}

			if ( beginChar == '>' )
			{
				// Low byte selector.
				reader.ReadChar();

				return Token(
					TokenType::LowSelector,
					beginLine,
					beginCol,
					ExtractString(reader, beginPos, beginPos + 1)
				);
			}

			if ( beginChar == '~' )
			{
				// Distance to a label.
				reader.ReadChar();

				return Token(
					TokenType::DistanceSelector,
					beginLine,
					beginCol,
					ExtractString(reader, beginPos, beginPos + 1)
				);
			}

			if ( beginChar == ':' )
			{
				// Label.
				return Token(TokenType::Label, beginLine, beginCol, ExtractLabel(reader));
			}

			if ( IsLetterOrUnderscore(beginChar) )
			{
				// Alphanumeric string.
				reader.SkipUntilAnyOf(
					[](char ch) -> bool
					{
						return !IsLetterNumberOrUnderscore(ch);
					}
				);

				size_t endPos = reader.GetCurrentPosition();

				return Token(
					TokenType::AlnumString,
					beginLine,
					beginCol,
					ExtractString(reader, beginPos, endPos)
				);
			}

			// If we reach here, the token was not recognised.
			break;
		}

		// Token was not recognised, so treat it as anything up until the next whitespace.
		size_t beginLine = reader.GetCurrentLine();
		size_t beginCol = reader.GetCurrentColumn();
		size_t beginPos = reader.GetCurrentPosition();
		reader.SkipUntilAnyOf(ANY_WHITESPACE);

		throw TokeniserException(
			reader,
			PublicErrorID::UNRECOGNISED_TOKEN,
			beginLine,
			beginCol,
			"Unrecognised token: " + ExtractString(reader, beginPos, reader.GetCurrentPosition()),
			TokeniserException::SkipBehaviour::NO_SKIP
		);
	}

	std::string Tokeniser::ExtractString(InputReader& reader, size_t begin, size_t end)
	{
		return begin < end ? std::string(reader.GetDataAtAbsolutePosition(begin), end - begin) : std::string();
	}

	std::string Tokeniser::ExtractNumericLiteral(InputReader& reader) const
	{
		size_t beginPos = reader.GetCurrentPosition();
		const char* validDigits = VALID_DIGITS_DEC;

		// We need to try and find the first character of the actual numerical value,
		// after sign/base characters.
		size_t beginOffset = 0;
		char beginChar = reader.PeekChar(beginOffset);

		// Skip a +/- prefix if it is present.
		if ( beginChar == '+' || beginChar == '-' )
		{
			beginChar = reader.PeekChar(++beginOffset);
		}

		if ( beginChar == '0' )
		{
			// See if the next digit is a base signifier.
			const char baseChar = reader.PeekChar(beginOffset + 1);

			if ( baseChar == 'x' || baseChar == 'X' )
			{
				validDigits = VALID_DIGITS_HEX;
				beginOffset += 2;
			}
			else if ( baseChar == 'b' || baseChar == 'B' )
			{
				validDigits = VALID_DIGITS_BIN;
				beginOffset += 2;
			}
		}

		reader.SkipChars(beginOffset);
		reader.SkipForAnyOf(validDigits);

		size_t endPos = reader.GetCurrentPosition();

		if ( endPos <= beginPos )
		{
			throw TokeniserException(
				reader,
				PublicErrorID::INVALID_NUMERIC_LITERAL,
				"Zero-length numeric literal.",
				TokeniserException::SkipBehaviour::NO_SKIP
			);
		}

		size_t length = endPos - beginPos;

		if ( length <= beginOffset )
		{
			// We only had a prefix but no actual digits.
			throw TokeniserException(
				reader,
				PublicErrorID::INVALID_NUMERIC_LITERAL,
				"Numeric literal prefix with no digits.",
				TokeniserException::SkipBehaviour::NO_SKIP
			);
		}

		return ExtractString(reader, beginPos, endPos);
	}

	std::string Tokeniser::ExtractLabel(InputReader& reader) const
	{
		size_t beginPos = reader.GetCurrentPosition();

		// Skip the initial :.
		reader.ReadChar();

		if ( std::isdigit(reader.PeekChar()) )
		{
			// Beginning of a label reference cannot be a number.
			throw TokeniserException(
				reader,
				PublicErrorID::INVALID_LABEL_NAME,
				"Label name cannot begin with a number.",
				TokeniserException::SkipBehaviour::ALNUM_OR_UNDERSCORE);
		}

		reader.SkipForAnyOf(&IsLetterNumberOrUnderscore);

		size_t endPos = reader.GetCurrentPosition();

		// We have +1 here because of the initial :.
		if ( endPos <= beginPos + 1 )
		{
			throw TokeniserException(
				reader,
				PublicErrorID::INVALID_LABEL_NAME,
				"Zero-length label name.",
				TokeniserException::SkipBehaviour::NO_SKIP);
		}

		return ExtractString(reader, beginPos, endPos);
	}
}
