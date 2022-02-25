#include <stddef.h>
#include "Tokens/TokenMeta_Label.h"

static const char* FindEndOfToken(const char* token)
{
	// Skip first "["
	++token;

	while ( *token )
	{
		if ( *token == ']' )
		{
			return token + 1;
		}

		++token;
	}

	// Token was unterminated.
	return NULL;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_Label =
{
	TOKEN_LABEL,
	"Label",

	&FindEndOfToken
};
