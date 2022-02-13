#include <stddef.h>
#include <ctype.h>
#include "Tokens/TokenMeta_Preprocessor.h"

static const char* FindEndOfToken(const char* token, V2MPAsm_TokenContext context)
{
	(void)context;

	// Skip first "#"
	token += 1;

	while ( *token )
	{
		if ( !BaseUtil_String_CharIsAlphanumericOrUnderscore(*token) )
		{
			return token;
		}

		++token;
	}

	// Ending a preprocessor with EOF is fine.
	return token;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_Preprocessor =
{
	TOKEN_PREPROCESSOR,
	"Preprocessor",

	&FindEndOfToken
};
