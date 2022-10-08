#pragma once

#include <string>

namespace V2MPAsm
{
	class InputReader;

	class Tokeniser
	{
	public:
		enum TokenType
		{
			EndOfFile = 1 << 0,
			EndOfLine = 1 << 1,
			AlnumString = 1 << 2,
			NumericLiteral = 1 << 3,
			LabelDefinition = 1 << 4,
			LabelReference = 1 << 5
		};

		struct Token
		{
			TokenType type;
			std::string token;
		};

		Token EmitToken(InputReader& reader);
	};
}
