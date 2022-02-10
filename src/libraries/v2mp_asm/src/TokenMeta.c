#include "BaseUtil/Util.h"
#include "TokenMeta.h"

static V2MPAsm_TokenType IdentifyTokenInDefaultContext(const char* str)
{
	if ( *str == '#' )
	{
		return TOKEN_PREPROCESSOR;
	}

	if ( *str == '/' )
	{
		if ( *(str + 1) == '/' )
		{
			return TOKEN_LINE_COMMENT;
		}

		if ( *(str + 1) == '*' )
		{
			return TOKEN_MULTILINE_COMMENT;
		}
	}

	if ( (*str >= '0' && *str <= '9') || *str == '-' || *str == '+' )
	{
		return TOKEN_NUMERIC_LITERAL;
	}

	if ( (*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z') || *str == '_' )
	{
		return TOKEN_NAME;
	}

	return TOKEN_UNKNOWN;
}

const char* V2MPAsm_TokenMeta_GetTokenTypeString(V2MPAsm_TokenType tokenType)
{
#define LIST_ITEM(value, name) name,
	static const char* const TYPE_STRINGS[] =
	{
		V2MPASM_TOKEN_TYPE_LIST
	};
#undef LIST_ITEM

	return (size_t)tokenType < BASEUTIL_ARRAY_SIZE(TYPE_STRINGS)
		? TYPE_STRINGS[(size_t)tokenType]
		: "<INVALID>";
}

const char* V2MPAsm_TokenMeta_GetTokenContextString(V2MPAsm_TokenContext tokencontext)
{
#define LIST_ITEM(value, name) name,
	static const char* const CONTEXT_STRINGS[] =
	{
		V2MPASM_TOKEN_CONTEXT_LIST
	};
#undef LIST_ITEM

	return (size_t)tokencontext < BASEUTIL_ARRAY_SIZE(CONTEXT_STRINGS)
		? CONTEXT_STRINGS[(size_t)tokencontext]
		: "<INVALID>";
}

V2MPAsm_TokenType V2MPAsm_TokenMeta_IdentifyToken(const char* str, V2MPAsm_TokenContext context)
{
	if ( !str || !(*str) )
	{
		return TOKEN_UNKNOWN;
	}

	switch ( context )
	{
		default:
		{
			return IdentifyTokenInDefaultContext(str);
		}
	}
}
