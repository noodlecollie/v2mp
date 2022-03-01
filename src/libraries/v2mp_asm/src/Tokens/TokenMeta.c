#include "BaseUtil/Util.h"
#include "BaseUtil/String.h"
#include "Tokens/TokenMeta.h"
#include "Tokens/TokenMeta_LineComment.h"
#include "Tokens/TokenMeta_MultilineComment.h"
#include "Tokens/TokenMeta_NumericLiteral.h"
#include "Tokens/TokenMeta_Name.h"
#include "Tokens/TokenMeta_Label.h"
#include "Tokens/TokenMeta_LabelReference.h"

#define LIST_ITEM(value, metaEntry) metaEntry,
static const V2MPAsm_TokenMeta* const TOKEN_METADATA[] =
{
	V2MPASM_TOKEN_TYPE_LIST
};
#undef LIST_ITEM

const char* V2MPAsm_TokenMeta_GetTokenTypeString(V2MPAsm_TokenType tokenType)
{
	const V2MPAsm_TokenMeta* meta;

	meta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);
	return meta ? meta->typeName : "Unknown";
}

V2MPAsm_TokenType V2MPAsm_TokenMeta_IdentifyToken(const char* str)
{
	size_t index;

	for ( index = 0; index < BASEUTIL_ARRAY_SIZE(TOKEN_METADATA); ++index )
	{
		if ( TOKEN_METADATA[index] &&
		     TOKEN_METADATA[index]->isTokenOfThisType &&
		     TOKEN_METADATA[index]->isTokenOfThisType(str) )
		{
			return TOKEN_METADATA[index]->type;
		}
	}

	return TOKEN_UNKNOWN;
}

const V2MPAsm_TokenMeta* V2MPAsm_TokenMeta_GetMetaForTokenType(V2MPAsm_TokenType tokenType)
{
	return (size_t)tokenType < BASEUTIL_ARRAY_SIZE(TOKEN_METADATA)
		? TOKEN_METADATA[(size_t)tokenType]
		: NULL;
}

bool V2MPAsm_TokenMeta_IsComment(V2MPAsm_TokenType tokenType)
{
	return tokenType == TOKEN_LINE_COMMENT || tokenType == TOKEN_MULTILINE_COMMENT;
}

const char* V2MPAsm_TokenMeta_FindEndOfToken(
	const V2MPAsm_TokenMeta* metadata,
	const char* token
)
{
	if ( !token )
	{
		return NULL;
	}

	if ( metadata && metadata->findEndOfToken )
	{
		return metadata->findEndOfToken(token);
	}
	else
	{
		return BaseUtil_String_NextWhitespace(token);
	}
}
