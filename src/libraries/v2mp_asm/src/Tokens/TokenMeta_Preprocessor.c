#include <stddef.h>
#include <ctype.h>
#include "Tokens/TokenMeta_Preprocessor.h"
#include "BaseUtil/String.h"

static const char* FindEndOfToken(const char* token)
{
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
