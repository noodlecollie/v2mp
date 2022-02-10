#include <stddef.h>
#include "Tokens/TokenMeta_LineComment.h"

static const char* FindEndOfToken(const char* token, V2MPAsm_TokenContext context)
{
	(void)context;

	while ( *token && *token != '\n' )
	{
		++token;
	}

	return token;
}

const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_LineComment =
{
	TOKEN_LINE_COMMENT,
	"Line Comment",

	&FindEndOfToken
};
