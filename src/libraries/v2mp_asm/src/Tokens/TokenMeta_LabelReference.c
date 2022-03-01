#include <stddef.h>
#include "Tokens/TokenMeta_LabelReference.h"
#include "BaseUtil/String.h"

static bool IsTokenOfThisType(const char* token)
{
	return
		(token[0] == '~' || token[0] == '+' || token[0] == '-') &&
		token[1] == ':';
}

static const char* FindEndOfToken(const char* token)
{
	// Skip first "+", "-" or "~"
	++token;

	while ( *token )
	{
		if ( !BaseUtil_String_CharIsAlphanumericOrUnderscore(*token) )
		{
			return token;
		}

		++token;
	}

	// Ending with an EOF is fine.
	return token;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_LabelReference =
{
	TOKEN_LABEL_REFERENCE,
	"LabelReference",

	&IsTokenOfThisType,
	&FindEndOfToken
};
