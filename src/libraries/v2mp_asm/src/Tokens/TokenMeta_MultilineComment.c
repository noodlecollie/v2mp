#include <stddef.h>
#include "Tokens/TokenMeta_MultilineComment.h"

static const char* FindEndOfToken(const char* token, V2MPAsm_TokenContext context)
{
	(void)context;

	while ( *token )
	{
		if ( *token == '*' && *(token + 1) == '/' )
		{
			token += 2;
			break;
		}

		++token;
	}

	return token;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_MultilineComment =
{
	TOKEN_MULTILINE_COMMENT,
	"Multiline Comment",

	&FindEndOfToken
};
