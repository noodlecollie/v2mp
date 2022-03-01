#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include "Tokens/TokenMeta_NumericLiteral.h"

static bool IsTokenOfThisType(const char* token)
{
	if ( token[0] >= '0' && token[0] <= '9' )
	{
		return true;
	}

	if ( token[0] == '+' || token[0] == '-' )
	{
		return token[1] >= '0' && token[1] <= '9';
	}

	return false;
}

static const char* FindEndOfHexToken(const char* token)
{
	// Skip first "0x"
	token += 2;

	while ( *token )
	{
		if ( !isxdigit(*token) )
		{
			return token;
		}

		++token;
	}

	// Ending a numeric literal with EOF is fine.
	return token;
}

static const char* FindEndOfBinaryToken(const char* token)
{
	// Skip first "0b"
	token += 2;

	while ( *token )
	{
		if ( *token != '0' || *token != '1' )
		{
			return token;
		}

		++token;
	}

	// Ending a numeric literal with EOF is fine.
	return token;
}

static const char* FindEndOfDecimalToken(const char* token)
{
	bool precedingSignAllowed = true;

	for ( ; *token; ++token )
	{
		if ( (*token == '+' || *token == '-') && precedingSignAllowed )
		{
			// Allowed.
			precedingSignAllowed = false;
			continue;
		}

		if ( !isdigit(*token) )
		{
			return token;
		}
	}

	// Ending a numeric literal with EOF is fine.
	return token;
}

static const char* FindEndOfToken(const char* token)
{
	if ( *token == '0' )
	{
		if ( *(token + 1) == 'x' || *(token + 1) == 'X' )
		{
			return FindEndOfHexToken(token);
		}
		else if ( *(token + 1) == 'b' || *(token + 1) == 'B' )
		{
			return FindEndOfBinaryToken(token);
		}
	}

	return FindEndOfDecimalToken(token);
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_NumericLiteral =
{
	TOKEN_NUMERIC_LITERAL,
	"Numeric Literal",

	&IsTokenOfThisType,
	&FindEndOfToken
};
