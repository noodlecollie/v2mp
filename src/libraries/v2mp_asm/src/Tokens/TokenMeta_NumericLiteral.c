#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include "Tokens/TokenMeta_NumericLiteral.h"

static const char* FindEndOfHexToken(const char* token, V2MPAsm_TokenContext context)
{
	(void)context;

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

static const char* FindEndOfBinaryToken(const char* token, V2MPAsm_TokenContext context)
{
	(void)context;

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

static const char* FindEndOfDecimalToken(const char* token, V2MPAsm_TokenContext context)
{
	bool precedingSignAllowed = true;

	(void)context;

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

static const char* FindEndOfToken(const char* token, V2MPAsm_TokenContext context)
{
	if ( *token == '0' )
	{
		if ( *(token + 1) == 'x' || *(token + 1) == 'X' )
		{
			return FindEndOfHexToken(token, context);
		}
		else if ( *(token + 1) == 'b' || *(token + 1) == 'B' )
		{
			return FindEndOfBinaryToken(token, context);
		}
	}

	return FindEndOfDecimalToken(token, context);
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_NumericLiteral =
{
	TOKEN_NUMERIC_LITERAL,
	"Numeric Literal",

	&FindEndOfToken
};
