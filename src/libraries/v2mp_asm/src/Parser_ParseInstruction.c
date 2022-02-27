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

static const char* ProcessInstructionArgument(V2MPAsm_ParseContext* context)
{
	V2MPAsm_ParseContext_TerminateWithError(
		context,
		PARSEERROR_UNIMPLEMENTED,
		"Implement parsing of instruction arguments."
	);

	return NULL;
}

void V2MPAsm_Parser_ParseInstruction(V2MPAsm_Parser* parser)
{
	const char* beginningOfInstruction;
	size_t beginLineNo;
	V2MPAsm_CWDInstruction* instructionCWD;
	const V2MPAsm_InstructionMeta* instructionMeta;
	size_t totalArgs;
	size_t argsProcessed;

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

	beginningOfInstruction = V2MPAsm_ParseContext_GetInputCursor(parser->context);

	if ( !CreateInitialInstruction(parser->context) )
	{
		return;
	}

	beginLineNo = V2MPAsm_ParseContext_GetInputLineNumber(parser->context);
	instructionCWD = V2MPAsm_CWDInstruction_Cast(V2MPAsm_ParseContext_GetCurrentCWD(parser->context));
	instructionMeta = V2MPAsm_CWDInstruction_GetInstructionMeta(instructionCWD);
	totalArgs = V2MPAsm_InstructionMeta_GetArgCount(instructionMeta);

	for ( argsProcessed = 0;
	      V2MPAsm_ParseContext_GetInputLineNumber(parser->context) == beginLineNo;
	      ++argsProcessed)
	{
		const char* endOfToken = NULL;

		if ( argsProcessed < totalArgs )
		{
			endOfToken = ProcessInstructionArgument(parser->context);
		}
		else
		{
			const char* begin;
			V2MPAsm_TokenType tokenType;
			const V2MPAsm_TokenMeta* tokenMeta;

			begin = V2MPAsm_ParseContext_GetInputCursor(parser->context);
			tokenType = V2MPAsm_TokenMeta_IdentifyToken(begin);
			tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);
			endOfToken = V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, begin);

			if ( !endOfToken )
			{
				V2MPAsm_ParseContext_TerminateWithError(
					parser->context,
					PARSEERROR_INTERNAL,
					"Internal error: could not determine end of token."
				);
			}
		}

		if ( !endOfToken )
		{
			// Something went wrong and an error has been set.
			return;
		}

		V2MPAsm_ParseContext_SeekInput(parser->context, endOfToken);
		V2MPAsm_ParseContext_SkipWhitespace(parser->context);
	}

	if ( argsProcessed != totalArgs )
	{
		V2MPAsm_ParseContext_SeekInput(parser->context, beginningOfInstruction);

		V2MPAsm_ParseContext_TerminateWithError(
			parser->context,
			PARSEERROR_INSUFFICIENT_ARGS,
			"Expected %zu %s for \"%s\" instruction, but was given %zu %s.",
			totalArgs,
			totalArgs == 1 ? "argument" : "arguments",
			V2MPAsm_InstructionMeta_GetName(instructionMeta),
			argsProcessed,
			argsProcessed == 1 ? "argument" : "arguments"
		);

		return;
	}
}
