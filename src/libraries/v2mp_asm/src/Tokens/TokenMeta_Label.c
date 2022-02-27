#include <stddef.h>
#include "Tokens/TokenMeta_Label.h"
#include "BaseUtil/String.h"

static const char* FindEndOfToken(const char* token)
{
	// Skip first ":"
	++token;

	while ( *token )
	{
		if ( !BaseUtil_String_CharIsAlphanumericOrUnderscore(*token) )
		{
			return token;
		}

		++token;
	}

	// Ending with an EOF is fine (although useless).
	return token;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_Label =
{
	TOKEN_LABEL,
	"Label",

	&FindEndOfToken
};
