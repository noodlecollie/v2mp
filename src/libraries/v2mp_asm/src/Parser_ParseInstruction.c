#include "Parser_ParseInstruction.h"
#include "Instructions/InstructionMeta.h"
#include "ParseException_Internal.h"
#include "CodewordDescriptors/CWD_Instruction.h"
#include "Tokens/TokenMeta.h"

static bool AddTokenToList(V2MPAsm_ParseContext* context)
{
	V2MPAsm_TokenList* tokenList;
	V2MPAsm_TokenType tokenType;
	const V2MPAsm_TokenMeta* tokenMeta;
	const char* begin;
	const char* end;

	tokenList = V2MPAsm_ParseContext_GetTokenList(context);

	V2MPAsm_ParseContext_SkipWhitespace(context);
	begin = V2MPAsm_ParseContext_GetInputCursor(context);

	if ( !begin )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: token cursor was not available."
		);

		return false;
	}

	tokenType = V2MPAsm_TokenMeta_IdentifyToken(begin);

	if ( tokenType == TOKEN_UNKNOWN )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNRECOGNISED_TOKEN,
			"Unrecognised token encountered."
		);

		return false;
	}

	tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);

	if ( !tokenMeta )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: could not get token metadata for \"%s\" token.",
			V2MPAsm_TokenMeta_GetTokenTypeString(tokenType)
		);

		return false;
	}

	end = V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, begin);

	if ( !end || end - begin < 1 )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: parsed a token with zero length."
		);

		return false;
	}

	if ( !V2MPAsm_TokenList_AppendNewEntry(tokenList, begin, end - begin) )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: failed to add token to list of tokens parsed for current instruction."
		);

		return false;
	}

	V2MPAsm_ParseContext_SeekInput(context, end);
	return true;
}

static bool PopulateTokenList(V2MPAsm_ParseContext* context)
{
	V2MPAsm_TokenList* tokenList;
	const char* end;

	tokenList = V2MPAsm_ParseContext_GetTokenList(context);
	V2MPAsm_TokenList_Clear(tokenList);

	end = V2MPAsm_ParseContext_GetEndOfCurrentLine(context);

	if ( !end )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: failed to process current line."
		);

		return false;
	}

	while ( V2MPAsm_ParseContext_GetInputCursor(context) < end )
	{
		if ( !AddTokenToList(context) )
		{
			return false;
		}
	}

	return true;
}

static bool CreateInitialInstruction(V2MPAsm_ParseContext* context)
{
	V2MPAsm_TokenList* tokenList;
	char* token;
	size_t tokenLength;
	const V2MPAsm_InstructionMeta* instructionMeta;
	V2MPAsm_CWDInstruction* instructionCWD;

	tokenList = V2MPAsm_ParseContext_GetTokenList(context);
	token = V2MPAsm_TokenListEntry_GetToken(V2MPAsm_TokenList_GetFirstEntry(tokenList));
	tokenLength = V2MPAsm_TokenListEntry_GetTokenLength(V2MPAsm_TokenList_GetFirstEntry(tokenList));

	if ( !token || tokenLength < 1 )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: invalid token for instruction name."
		);

		return false;
	}

	instructionMeta = V2MPAsm_InstructionMeta_FindCommand(token, tokenLength);

	if ( !instructionMeta )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNRECOGNISED_INSTRUCTION,
			"\"%s\" was not recognised as a CPU instruction.",
			token
		);

		return false;
	}

	instructionCWD = V2MPAsm_CWDInstruction_Cast(
		V2MPAsm_ParseContext_AppendNewCWDAsCurrent(context, V2MPASM_CWD_INSTRUCTION)
	);

	if ( !instructionCWD )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: could not create instruction data for \"%s\"",
			token
		);

		return false;
	}

	V2MPAsm_CWDInstruction_SetInstructionMeta(instructionCWD, instructionMeta);
	return true;
}

void V2MPAsm_Parser_ParseInstruction(V2MPAsm_Parser* parser)
{
	// Sanity:
	if ( V2MPAsm_ParseContext_GetParseState(parser->context) != PARSESTATE_BUILDING_INSTRUCTION )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			parser->context,
			PARSEERROR_INTERNAL,
			"Internal error: instruction parsing routines triggered when the parser was in an incorrect state."
		);

		return;
	}

	if ( !PopulateTokenList(parser->context) ||
	     !CreateInitialInstruction(parser->context) )
	{
		return;
	}

	V2MPAsm_ParseContext_TerminateWithError(
		parser->context,
		PARSEERROR_UNIMPLEMENTED,
		"Instruction parsing still to be implemented."
	);
}
