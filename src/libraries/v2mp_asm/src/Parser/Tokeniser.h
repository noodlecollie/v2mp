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
#define TOKEN_TYPE_LIST \
	LIST_ITEM(EndOfFile, 1 << 0, "EndOfFile") \
	LIST_ITEM(EndOfLine, 1 << 1, "EndOfLine") \
	LIST_ITEM(AlnumString, 1 << 2, "AlnumString") \
	LIST_ITEM(NumericLiteral, 1 << 3, "NumericLiteral") \
	LIST_ITEM(Label, 1 << 4, "Label") \
	LIST_ITEM(HighSelector, 1 << 5, "HighSelector") \
	LIST_ITEM(LowSelector, 1 << 6, "LowSelector") \
	LIST_ITEM(DistanceSelector, 1 << 7, "DistanceSelector") \
	LIST_ITEM(PreprocessorCommand, 1 << 8, "PreprocessorCommand")

		enum TokenType
		{
#define LIST_ITEM(enumName, value, desc) enumName = value,
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
		Token EmitToken(InputReader& reader) const;

	private:
		class TokeniserException : public AssemblerException
		{
		public:
			TokeniserException(InputReader& reader, PublicErrorID errorID, const std::string& message);
			TokeniserException(InputReader& reader, PublicWarningID errorID, const std::string& message);

			TokeniserException(
				InputReader& reader,
				PublicErrorID errorID,
				size_t line,
				size_t column,
				const std::string& message
			);

			TokeniserException(
				InputReader& reader,
				PublicWarningID errorID,
				size_t line,
				size_t column,
				const std::string& message
			);
		};

		static std::string ExtractString(InputReader& reader, size_t begin = 0, size_t end = 0);

		std::string ExtractNumericLiteral(InputReader& reader) const;
		std::string ExtractLabel(InputReader& reader) const;
	};
}
