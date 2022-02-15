#include "BaseUtil/Util.h"
#include "Parser_Routines.h"
#include "ParseException_Internal.h"
#include "ParseException_Internal.h"
#include "Tokens/TokenMeta.h"
#include "Instructions/InstructionMeta.h"

// REMOVE ME
#include <stdio.h>

static void InitResources()
{
	V2MPAsm_InstructionMeta_InitTable();
}

static void DestroyResources()
{
	V2MPAsm_InstructionMeta_DestroyTable();
}

static void SkipToken(V2MPAsm_ParseContext* context, V2MPAsm_TokenType tokenType)
{
	const char* end;

	end = V2MPAsm_TokenMeta_FindEndOfToken(
		V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType),
		V2MPAsm_ParseContext_GetInputCursor(context),
		V2MPAsm_ParseContext_GetTokenContext(context)
	);

	V2MPAsm_ParseContext_SeekInput(context, end);
}

static void ParseName(V2MPAsm_ParseContext* context)
{
	const char* begin;
	const char* end;
	const V2MPAsm_InstructionMeta* instructionMeta;

	begin = V2MPAsm_ParseContext_GetInputCursor(context);

	end = V2MPAsm_TokenMeta_FindEndOfToken(
		V2MPAsm_TokenMeta_GetMetaForTokenType(TOKEN_NAME),
		begin,
		V2MPAsm_ParseContext_GetTokenContext(context)
	);

	if ( !V2MPAsm_ParseContext_SetCurrentToken(context, begin, end) )
	{
		V2MPAsm_ParseContext_CreateAndSetError(
			context,
			PARSEERROR_INTERNAL,
			"Could not allocate memory to parse token of length %zu characters.",
			BASEUTIL_MAX((size_t)(end - begin), 0)
		);

		V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
		return;
	}

	instructionMeta = V2MPAsm_InstructionMeta_FindCommand(
		V2MPAsm_ParseContext_GetCurrentToken(context),
		V2MPAsm_ParseContext_GetCurrentTokenLength(context)
	);

	if ( instructionMeta )
	{
		// TODO: Handle this properly
		printf("Parsed instruction name: %s\n", instructionMeta->name);

		V2MPAsm_ParseContext_SeekInput(context, end);
	}
	else
	{
		V2MPAsm_ParseContext_CreateAndSetError(
			context,
			PARSEERROR_UNRECOGNISED_INSTRUCTION,
			"\"%s\" was not recognised as a CPU instruction or an alias.",
			V2MPAsm_ParseContext_GetCurrentToken(context)
		);

		V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
	}
}

static void ParseDefault(V2MPAsm_ParseContext* context)
{
	const char* token;
	V2MPAsm_TokenType tokenType;

	token = V2MPAsm_ParseContext_GetBeginningOfNextToken(context);
	tokenType = V2MPAsm_TokenMeta_IdentifyToken(token, V2MPAsm_ParseContext_GetTokenContext(context));

	if ( V2MPAsm_TokenMeta_IsComment(tokenType)  )
	{
		SkipToken(context, tokenType);
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

	ParseName(context);
}

void V2MPAsm_Parser_ExecuteParse(V2MPAsm_Parser* parser)
{
	if ( !parser )
	{
		return;
	}

	InitResources();

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

	DestroyResources();
}
