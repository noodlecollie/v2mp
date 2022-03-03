#include <stddef.h>
#include "Tokens/TokenMeta_LabelReference.h"
#include "BaseUtil/String.h"

#define CHAR_DISTANCE '~'
#define CHAR_MSB '+'
#define CHAR_LSB '-'

static bool IsTokenOfThisType(const char* token)
{
	return
		(token[0] == CHAR_DISTANCE || token[0] == CHAR_MSB || token[0] == CHAR_LSB) &&
		token[1] == ':';
}

static const char* FindEndOfToken(const char* token)
{
	// Skip first identifier tokens.
	token += 2;

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

V2MPAsm_LabelReferenceType V2MPAsm_LabelReference_GetReferenceType(const char* token)
{
	if ( !token )
	{
		return LABELREF_INVALID;
	}

	switch ( *token )
	{
		case CHAR_DISTANCE:
		{
			return LABELREF_DISTANCE;
		}

		case CHAR_MSB:
		{
			return LABELREF_MSB;
		}

		case CHAR_LSB:
		{
			return LABELREF_LSB;
		}

		default:
		{
			return LABELREF_INVALID;
		}
	}
}

const char* V2MPAsm_LabelReference_GetLabelName(const char* token)
{
	if ( !token || !IsTokenOfThisType(token) )
	{
		return NULL;
	}

	token += 2;

	return BaseUtil_String_CharIsAlphanumericOrUnderscore(*token) ? token : NULL;
}
