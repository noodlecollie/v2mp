#include <stdarg.h>
#include "Parser_ParseInstruction.h"
#include "Instructions/InstructionMeta.h"
#include "ParseException_Internal.h"
#include "CodewordDescriptors/CWD_Instruction.h"
#include "Tokens/TokenMeta.h"
#include "BaseUtil/Heap.h"

static void TerminateWithErrorV(
	V2MPAsm_ParseContext* context,
	V2MPAsm_ParseErrorType errorType,
	const V2MPAsm_TokenListEntry* tokenEntry,
	const char* format,
	va_list args
)
{
	V2MPAsm_ParseContext_ExceptionNode* node;

	node = V2MPAsm_ParseContext_AppendException(context);

	if ( !node )
	{
		// Should never happen, but what else can we do?
		return;
	}

	V2MPAsm_ParseException_SetError(node->exception, errorType);

	V2MPAsm_ParseException_SetLineAndColumn(node->exception,
		V2MPAsm_TokenListEntry_GetTokenLine(tokenEntry),
		V2MPAsm_TokenListEntry_GetTokenColumn(tokenEntry)
	);

	V2MPAsm_ParseException_FormatCustomDescriptionV(node->exception, format, args);

	V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
}

static inline void TerminateWithError(
	V2MPAsm_ParseContext* context,
	V2MPAsm_ParseErrorType errorType,
	const V2MPAsm_TokenListEntry* tokenEntry,
	const char* format,
	...
)
{
	va_list list;
	va_start(list, format);

	TerminateWithErrorV(
		context,
		errorType,
		tokenEntry,
		format,
		list
	);

	va_end(list);
}

static bool AddTokenToList(V2MPAsm_ParseContext* context)
{
	V2MPAsm_TokenList* tokenList;
	V2MPAsm_TokenType tokenType;
	const V2MPAsm_TokenMeta* tokenMeta;
	const char* begin;
	const char* end;
	V2MPAsm_TokenListEntry* tokenEntry;

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
		char* buffer = NULL;
		const char* tokenEnd;

		tokenEnd = V2MPAsm_TokenMeta_FindEndOfToken(
			V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType),
			begin
		);

		if ( tokenEnd )
		{
			buffer = V2MPAsm_ParseContext_CopyFromInputCursor(context, tokenEnd - begin);
		}

		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNRECOGNISED_TOKEN,
			"Unrecognised token encountered: \"%s\"",
			buffer ? buffer : ""
		);

		if ( buffer )
		{
			BASEUTIL_FREE(buffer);
		}

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

	tokenEntry = V2MPAsm_TokenList_AppendNewEntry(tokenList, begin, end - begin);

	if ( !tokenEntry )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INTERNAL,
			"Internal error: failed to add token to list of tokens parsed for current instruction."
		);

		return false;
	}

	V2MPAsm_TokenListEntry_SetTokenLine(tokenEntry, V2MPAsm_ParseContext_GetInputLineNumber(context));
	V2MPAsm_TokenListEntry_SetTokenColumn(tokenEntry, V2MPAsm_ParseContext_GetInputColumnNumber(context));

	V2MPAsm_ParseContext_SeekInput(context, end);
	return true;
}

static bool PopulateTokenList(V2MPAsm_ParseContext* context)
{
	V2MPAsm_TokenList* tokenList;
	const char* end;

	tokenList = V2MPAsm_ParseContext_GetTokenList(context);
	V2MPAsm_TokenList_Clear(tokenList);

	end = V2MPAsm_ParseContext_GetEndOfInputLine(context);

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

static bool AddInstructionArgument(V2MPAsm_ParseContext* context, V2MPAsm_TokenListEntry* tokenEntry)
{
	V2MPAsm_TokenType tokenType;

	tokenType = V2MPAsm_TokenMeta_IdentifyToken(tokenEntry->token);

	// TODO: Support more than just numbers
	if ( tokenType != TOKEN_NUMERIC_LITERAL )
	{
		TerminateWithError(
			context,
			PARSEERROR_UNEXPECTED_TOKEN,
			tokenEntry,
			"Expected a numeric literal but got \"%s\".",
			tokenEntry->token
		);

		return false;
	}

	// TODO: Validate range of argument

	// TODO: Add argument to CWD
	V2MPAsm_ParseContext_TerminateWithError(
		context,
		PARSEERROR_UNIMPLEMENTED,
		"Need to implement adding argument to instruction."
	);

	return false;
}

static bool PopulateInstructionArguments(V2MPAsm_ParseContext* context)
{
	V2MPAsm_TokenListEntry* tokenEntry;

	tokenEntry = V2MPAsm_TokenList_GetFirstEntry(V2MPAsm_ParseContext_GetTokenList(context));

	// Skip the first token as it's the instruction name.
	tokenEntry = V2MPAsm_TokenList_GetNextEntry(tokenEntry);

	for ( ; tokenEntry; tokenEntry = V2MPAsm_TokenList_GetNextEntry(tokenEntry) )
	{
		if ( !AddInstructionArgument(context, tokenEntry) )
		{
			return false;
		}
	}

	// TODO: Validate the codeword descriptor.
	V2MPAsm_ParseContext_TerminateWithError(
		context,
		PARSEERROR_UNIMPLEMENTED,
		"Implement validation of codeword descriptor."
	);

	return false;
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

	if ( !PopulateTokenList(parser->context) )
	{
		return;
	}

	if ( !CreateInitialInstruction(parser->context) )
	{
		return;
	}

	if ( !PopulateInstructionArguments(parser->context) )
	{
		return;
	}
}
