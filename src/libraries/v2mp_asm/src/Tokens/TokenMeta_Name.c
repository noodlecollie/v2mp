#include <stddef.h>
#include <ctype.h>
#include "Tokens/TokenMeta_Name.h"

static const char* FindEndOfToken(const char* token, V2MPAsm_TokenContext context)
{
	(void)context;

	while ( *token )
	{
		if ( !isalnum(*token) && *token != '_' )
		{
			return token;
		}

		++token;
	}

	// Ending a name with EOF is fine.
	return token;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_Name =
{
	TOKEN_NAME,
	"Name",

	&FindEndOfToken
};
