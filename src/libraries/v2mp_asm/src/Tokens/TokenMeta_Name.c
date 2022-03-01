#include <stddef.h>
#include <ctype.h>
#include "Tokens/TokenMeta_Name.h"
#include "BaseUtil/String.h"

static bool IsTokenOfThisType(const char* token)
{
	return (token[0] >= 'A' && token[0] <= 'Z') || (token[0] >= 'a' && token[0] <= 'z') || token[0] == '_';
}

static const char* FindEndOfToken(const char* token)
{
	while ( *token )
	{
		if ( !BaseUtil_String_CharIsAlphanumericOrUnderscore(*token) )
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

	&IsTokenOfThisType,
	&FindEndOfToken
};
