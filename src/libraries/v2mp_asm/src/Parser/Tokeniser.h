#pragma once

#include <string>
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
	class InputReader;

	class Tokeniser
	{
	public:
	// TODO: This is getting unwieldy. Create something like the instruction meta struct.
#define TOKEN_TYPE_LIST \
	LIST_ITEM(EndOfFile, 1 << 0, "EndOfFile", "EOF", false) \
	LIST_ITEM(EndOfLine, 1 << 1, "EndOfLine", "'\\n'", false) \
	LIST_ITEM(AlnumString, 1 << 2, "AlnumString", "string", true) \
	LIST_ITEM(NumericLiteral, 1 << 3, "NumericLiteral", "numeric literal", true) \
	LIST_ITEM(Label, 1 << 4, "Label", "label", true) \
	LIST_ITEM(HighSelector, 1 << 5, "HighSelector", "high byte selector", true) \
	LIST_ITEM(LowSelector, 1 << 6, "LowSelector", "low byte selector", true) \
	LIST_ITEM(DistanceSelector, 1 << 7, "DistanceSelector", "label distance selector", true) \
	LIST_ITEM(PreprocessorCommand, 1 << 8, "PreprocessorCommand", "preprocessor command", false)

		enum TokenType
		{
#define LIST_ITEM(enumName, value, name, desc, printable) enumName = value,
			TOKEN_TYPE_LIST
#undef LIST_ITEM
		};

		struct Token
		{
			TokenType type;
			size_t line;
			size_t column;
			std::string token;

			Token(TokenType inType, size_t inLine, size_t inColumn, const std::string& inToken = std::string()) :
				type(inType),
				line(inLine),
				column(inColumn),
				token(inToken)
			{
			}

			Token(TokenType inType, size_t inLine, size_t inColumn, std::string&& inToken) :
				type(inType),
				line(inLine),
				column(inColumn),
				token(std::move(inToken))
			{
			}
		};

		static std::string TokenName(TokenType tokenType);
		static std::string TokenReadableDescription(TokenType tokenType);
		static bool TokenIsPrintable(TokenType tokenType);

		static std::string TokenPrintableString(const Token& token);

		Token EmitToken(InputReader& reader) const;

	private:
		class TokeniserException : public AssemblerException
		{
		public:
			enum class SkipBehaviour
			{
				NO_SKIP,
				UNTIL_WHITESPACE,
				ALNUM_OR_UNDERSCORE
			};

			TokeniserException(
				InputReader& reader,
				PublicErrorID errorID,
				const std::string& message,
				SkipBehaviour inSkipBehaviour = SkipBehaviour::UNTIL_WHITESPACE
			);

			TokeniserException(
				InputReader& reader,
				PublicWarningID errorID,
				const std::string& message,
				SkipBehaviour inSkipBehaviour = SkipBehaviour::UNTIL_WHITESPACE
			);

			TokeniserException(
				InputReader& reader,
				PublicErrorID errorID,
				size_t line,
				size_t column,
				const std::string& message,
				SkipBehaviour inSkipBehaviour = SkipBehaviour::UNTIL_WHITESPACE
			);

			TokeniserException(
				InputReader& reader,
				PublicWarningID errorID,
				size_t line,
				size_t column,
				const std::string& message,
				SkipBehaviour inSkipBehaviour = SkipBehaviour::UNTIL_WHITESPACE
			);

			SkipBehaviour skipBehaviour;
		};

		static std::string ExtractString(InputReader& reader, size_t begin = 0, size_t end = 0);

		Token EmitTokenInternal(InputReader& reader) const;
		std::string ExtractNumericLiteral(InputReader& reader) const;
		std::string ExtractLabel(InputReader& reader) const;
	};
}
