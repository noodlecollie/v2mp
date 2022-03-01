#include <stddef.h>
#include "Tokens/TokenMeta_LineComment.h"

static bool IsTokenOfThisType(const char* token)
{
	return token[0] == '/' && token[1] == '/';
}

static const char* FindEndOfToken(const char* token)
{
	// Skip first "//"
	token += 2;

	while ( *token )
	{
		if ( *token == '\n' )
		{
			return token + 1;
		}

		++token;
	}

	// Ending a line comment with EOF is fine.
	return token;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_LineComment =
{
	TOKEN_LINE_COMMENT,
	"Line Comment",

	&IsTokenOfThisType,
	&FindEndOfToken
};
