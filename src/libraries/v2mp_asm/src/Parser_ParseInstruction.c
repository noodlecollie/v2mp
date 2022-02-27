#include <stdarg.h>
#include "Parser_ParseInstruction.h"
#include "Instructions/InstructionMeta.h"
#include "ParseException_Internal.h"
#include "CodewordDescriptors/CWD_Instruction.h"
#include "Tokens/TokenMeta.h"
#include "BaseUtil/Heap.h"

static bool CreateInitialInstruction(V2MPAsm_ParseContext* context)
{
	const char* begin;
	const char* end;
	V2MPAsm_TokenType tokenType;
	const V2MPAsm_TokenMeta* tokenMeta;
	const V2MPAsm_InstructionMeta* instructionMeta;
	V2MPAsm_CWDInstruction* instructionCWD;

	begin = V2MPAsm_ParseContext_GetInputCursor(context);
	tokenType = V2MPAsm_TokenMeta_IdentifyToken(begin);
	tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);
	end = V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, begin);

	if ( !begin )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"No token to read."
		);

		return false;
	}

	if ( !end )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNRECOGNISED_TOKEN,
			"Unterminated token of type \"%s\".",
			V2MPAsm_TokenMeta_GetTokenTypeString(tokenType)
		);

		return false;
	}

	begin = V2MPAsm_ParseContext_SetCurrentToken(context, begin, end);

	if ( !begin )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: could not allocate buffer for instruction name token."
		);

		return false;
	}

	if ( tokenType != TOKEN_NAME )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNEXPECTED_TOKEN,
			"Expected name of instruction but got \"%s\".",
			begin
		);

		return false;
	}

	instructionMeta = V2MPAsm_InstructionMeta_FindCommand(begin, V2MPAsm_ParseContext_GetCurrentTokenLength(context));

	if ( !instructionMeta )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNRECOGNISED_INSTRUCTION,
			"\"%s\" was not recognised as a valid instruction name.",
			begin
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
			begin
		);

		return false;
	}

	V2MPAsm_CWDInstruction_SetInstructionMeta(instructionCWD, instructionMeta);
	V2MPAsm_ParseContext_SeekInput(context, end);
	V2MPAsm_ParseContext_SkipWhitespace(context);

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

	if ( !CreateInitialInstruction(parser->context) )
	{
		return;
	}

	// TODO: Parse arguments
	V2MPAsm_ParseContext_TerminateWithError(
		parser->context,
		PARSEERROR_UNIMPLEMENTED,
		"Implement parsing of instruction arguments."
	);
}
