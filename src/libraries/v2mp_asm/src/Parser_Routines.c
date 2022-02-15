#include "Parser_Routines.h"
#include "ParseException_Internal.h"
#include "ParseException_Internal.h"
#include "Tokens/TokenMeta.h"

static void SkipToken(V2MPAsm_ParseContext* context, V2MPAsm_TokenType tokenType, V2MPAsm_TokenContext tokenContext)
{
	const char* end;

	end = V2MPAsm_TokenMeta_FindEndOfToken(
		V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType),
		V2MPAsm_ParseContext_GetInputCursor(context),
		tokenContext
	);

	V2MPAsm_ParseContext_SeekInput(context, end);
}

static void ParseDefault(V2MPAsm_ParseContext* context)
{
	const char* token;
	V2MPAsm_TokenType tokenType;

	token = V2MPAsm_ParseContext_GetBeginningOfNextToken(context);
	tokenType = V2MPAsm_TokenMeta_IdentifyToken(token, TOKENCTX_DEFAULT);

	if ( V2MPAsm_TokenMeta_IsComment(tokenType)  )
	{
		SkipToken(context, tokenType, TOKENCTX_DEFAULT);
		return;
	}

	if ( tokenType != TOKEN_NAME )
	{
		// TODO: Implement properly.
		V2MPAsm_ParseContext_CreateAndSetError(
			context,
			PARSEERROR_UNIMPLEMENTED,
			"Parsing token of type \"%s\" is not yet implemented.",
			V2MPAsm_TokenMeta_GetTokenTypeString(tokenType)
		);

		V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
		return;
	}

	V2MPAsm_ParseContext_CreateAndSetError(
			context,
			PARSEERROR_UNIMPLEMENTED,
			"Need to implement parsing for \"Name\" tokens."
		);

	V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
}

void V2MPAsm_Parser_ExecuteParse(V2MPAsm_Parser* parser)
{
	if ( !parser )
	{
		return;
	}

	while ( V2MPAsm_ParseContext_GetParseState(parser->context) != PARSESTATE_TERMINATED )
	{
		switch ( V2MPAsm_ParseContext_GetParseState(parser->context) )
		{
			case PARSESTATE_DEFAULT:
			{
				ParseDefault(parser->context);
				break;
			}

			default:
			{
				V2MPAsm_ParseContext_CreateAndSetError(parser->context, PARSEERROR_INTERNAL, "Unknown internal state encountered.");
				V2MPAsm_ParseContext_SetParseState(parser->context, PARSESTATE_TERMINATED);
				break;
			}
		}
	}
}
