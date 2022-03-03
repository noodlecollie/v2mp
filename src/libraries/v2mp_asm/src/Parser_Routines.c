#include "BaseUtil/Util.h"
#include "Parser_Routines.h"
#include "ParseException_Internal.h"
#include "Tokens/TokenMeta.h"
#include "Instructions/InstructionMeta.h"
#include "Parser_ParseInstruction.h"
#include "Parser_ParseLabel.h"

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
		V2MPAsm_ParseContext_GetInputCursor(context)
	);

	V2MPAsm_ParseContext_SeekInput(context, end);
}

static void ParseDefault(V2MPAsm_ParseContext* context)
{
	const char* token;
	V2MPAsm_TokenType tokenType;

	V2MPAsm_ParseContext_SkipWhitespace(context);

	if ( V2MPAsm_ParseContext_InputIsAtEOF(context) )
	{
		// TODO: Set state to final pass, where label references
		// are resolved, rather than terminating.
		V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
		return;
	}

	token = V2MPAsm_ParseContext_GetInputCursor(context);
	tokenType = V2MPAsm_TokenMeta_IdentifyToken(token);

	if ( V2MPAsm_TokenMeta_IsComment(tokenType) )
	{
		SkipToken(context, tokenType);
		return;
	}

	switch ( tokenType )
	{
		case TOKEN_NAME:
		{
			V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_BUILDING_INSTRUCTION);
			break;
		}

		case TOKEN_LABEL:
		{
			V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_RECORDING_LABEL);
			break;
		}

		// TODO: Implement support for other token types!
		default:
		{
			V2MPAsm_ParseContext_TerminateWithError(
				context,
				PARSEERROR_UNIMPLEMENTED,
				"Parsing token of type \"%s\" is not yet implemented.",
				V2MPAsm_TokenMeta_GetTokenTypeString(tokenType)
			);

			break;
		}
	}
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

			case PARSESTATE_BUILDING_INSTRUCTION:
			{
				V2MPAsm_Parser_ParseInstruction(parser);
				break;
			}

			case PARSESTATE_RECORDING_LABEL:
			{
				V2MPAsm_Parser_ParseLabel(parser);
				break;
			}

			default:
			{
				PARSECONTEXT_INTERNAL_ERROR(
					parser->context,
					"Unknown internal state encountered."
				);

				break;
			}
		}
	}

	DestroyResources();
}
