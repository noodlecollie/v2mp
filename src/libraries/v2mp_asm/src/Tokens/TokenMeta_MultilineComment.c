#include <stddef.h>
#include "Tokens/TokenMeta_MultilineComment.h"

static const char* FindEndOfToken(const char* token, V2MPAsm_TokenContext context)
{
	(void)context;

	// Skip first "/*"
	token += 2;

	while ( *token )
	{
		if ( *token == '*' && *(token + 1) == '/' )
		{
			return token + 2;
			break;
		}

		++token;
	}

	// Token was unterminated.
	return NULL;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_MultilineComment =
{
	TOKEN_MULTILINE_COMMENT,
	"Multiline Comment",

	&FindEndOfToken
};
