#include <stdarg.h>
#include "Parser_ParseInstruction.h"
#include "Parser_Internal.h"
#include "Instructions/InstructionMeta.h"
#include "ParseException_Internal.h"
#include "CodewordDescriptors/CWD_Instruction.h"
#include "Tokens/TokenMeta.h"
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "Tokens/TokenMeta_LabelReference.h"

static inline V2MPAsm_Word DetermineMaxValue(size_t numBits)
{
	if ( numBits < 1 )
	{
		return 0;
	}

	if ( numBits >= 16 )
	{
		return 0xFFFF;
	}

	return 0xFFFF >> (16 - numBits);
}

static bool CreateInitialInstruction(V2MPAsm_ParseContext* context)
{
	const char* begin;
	const char* end;
	V2MPAsm_TokenType tokenType;
	const V2MPAsm_TokenMeta* tokenMeta;
	const V2MPAsm_InstructionMeta* instructionMeta;
	V2MPAsm_CWDInstruction* instructionCWD;

	V2MPAsm_ParseContext_SkipWhitespace(context);
	begin = V2MPAsm_ParseContext_GetInputCursor(context);
	tokenType = V2MPAsm_TokenMeta_IdentifyToken(begin);
	tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);
	end = V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, begin);

	if ( !begin )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			context,
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
		PARSECONTEXT_INTERNAL_ERROR(
			context,
			"Could not allocate buffer for instruction name token."
		);

		return false;
	}

	if ( tokenType != TOKEN_NAME )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNEXPECTED_TOKEN,
			"Expected name of instruction, but got \"%s\".",
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
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not create instruction data for \"%s\"",
			begin
		);

		return false;
	}

	if ( !V2MPAsm_CWDInstruction_SetInstructionMeta(instructionCWD, instructionMeta) )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not initialise instruction data for \"%s\"",
			begin
		);

		return false;
	}

	V2MPAsm_ParseContext_SeekInput(context, end);

	return true;
}

static bool ValidateNumericalArg(V2MPAsm_ParseContext* context, V2MPAsm_CWDInstruction* cwd, size_t argIndex, V2MPAsm_Word arg)
{
	const V2MPAsm_InstructionMeta* instructionMeta;
	const V2MPAsm_InstructionArg* instructionArg;
	V2MPAsm_Word max = 0;

	instructionMeta = V2MPAsm_CWDInstruction_GetInstructionMeta(cwd);

	if ( !instructionMeta )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			context,
			"Could not fetch metadata for instruction."
		);

		return false;
	}

	instructionArg = V2MPAsm_InstructionMeta_GetArg(instructionMeta, argIndex);

	if ( !instructionArg )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not fetch metadata for instruction argument %zu. Instruction expects %zu arguments.",
			argIndex,
			V2MPAsm_InstructionMeta_GetArgCount(instructionMeta)
		);

		return false;
	}

	max = DetermineMaxValue(V2MPasm_InstructionArg_GetNumBits(instructionArg));

	if ( arg > max )
	{
		V2MPAsm_ParseContext_CreateAndSetWarning(
			context,
			PARSEWARNING_ARG_OUT_OF_RANGE,
			"Value of %u exceeds max allowed value of %u for argument. Value will be truncated.",
			arg,
			max
		);
	}

	return true;
}

static bool AddNumericArgument(V2MPAsm_ParseContext* context, V2MPAsm_CWDInstruction* instructionCWD, size_t argIndex)
{
	const char* begin;
	int base = 0;
	long int argAsNumber = 0;
	V2MPAsm_Word argAsWord = 0;
	V2MPAsm_CWDInstruction_Arg* instructionArg;

	begin = V2MPAsm_ParseContext_GetCurrentToken(context);
	base = BaseUtil_String_GetBaseFromNumberPrefix(begin);

	if ( base == 0 )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INVALID_NUMERIC_LITERAL,
			"Numeric literal \"%s\" specified an invalid base."
		);

		return false;
	}

	if ( !BaseUtil_String_ToLongInt(begin, NULL, base, &argAsNumber) )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_INVALID_NUMERIC_LITERAL,
			"Numeric literal \"%s\" did not represent a valid number."
		);

		return false;
	}

	argAsWord = (V2MPAsm_Word)argAsNumber;

	if ( !ValidateNumericalArg(context, instructionCWD, argIndex, argAsWord) )
	{
		// Error will have been set.
		return false;
	}

	instructionArg = V2MPAsm_CWDInstruction_GetInstructionArg(instructionCWD, argIndex);

	if ( !instructionArg )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not fetch instruction data when processing argument \"%s\"",
			begin
		);

		return false;
	}

	if ( !V2MPAsm_CWDInstructionArg_SetNumericValue(instructionArg, argAsWord) )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not set instruction data when processing argument \"%s\"",
			begin
		);

		return false;
	}

	return true;
}

static bool AddLabelRefArgument(V2MPAsm_ParseContext* context, V2MPAsm_CWDInstruction* instructionCWD, size_t argIndex)
{
	const char* begin;
	V2MPAsm_CWDInstruction_Arg* instructionArg;
	V2MPAsm_LabelReferenceType labelRefType;
	const char* labelName;

	begin = V2MPAsm_ParseContext_GetCurrentToken(context);
	instructionArg = V2MPAsm_CWDInstruction_GetInstructionArg(instructionCWD, argIndex);

	if ( !instructionArg )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not fetch instruction data when processing argument \"%s\"",
			begin
		);

		return false;
	}

	labelRefType = V2MPAsm_LabelReference_GetReferenceType(begin);

	if ( labelRefType == LABELREF_INVALID )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not deduce label reference type from \"%s\"",
			begin
		);

		return false;
	}

	labelName = V2MPAsm_LabelReference_GetLabelName(begin);

	if ( !labelName )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_EMPTY_TOKEN,
			"No label name was provided for this label reference."
		);

		return false;
	}

	if ( !V2MPAsm_CWDInstructionArg_SetLabelRef(instructionArg, labelName, labelRefType) )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not set instruction data when processing argument \"%s\"",
			begin
		);

		return false;
	}

	return true;
}

static const char* ProcessInstructionArgument(V2MPAsm_ParseContext* context, size_t argIndex)
{
	const char* begin;
	const char* end;
	V2MPAsm_TokenType tokenType;
	const V2MPAsm_TokenMeta* tokenMeta;
	V2MPAsm_CWDInstruction* instructionCWD;

	V2MPAsm_ParseContext_SkipWhitespace(context);
	begin = V2MPAsm_ParseContext_GetInputCursor(context);
	tokenType = V2MPAsm_TokenMeta_IdentifyToken(begin);
	tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);
	end = V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, begin);

	if ( !begin )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			context,
			"No token to read."
		);

		return NULL;
	}

	if ( !end )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			context,
			PARSEERROR_UNRECOGNISED_TOKEN,
			"Unterminated token of type \"%s\".",
			V2MPAsm_TokenMeta_GetTokenTypeString(tokenType)
		);

		return NULL;
	}

	begin = V2MPAsm_ParseContext_SetCurrentToken(context, begin, end);

	if ( !begin )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			context,
			"Could not allocate buffer for instruction name token."
		);

		return NULL;
	}

	instructionCWD = V2MPAsm_CWDInstruction_Cast(V2MPAsm_ParseContext_GetCurrentCWD(context));

	if ( !instructionCWD )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			context,
			"Could not create instruction data when processing argument \"%s\"",
			begin
		);

		return false;
	}

	switch ( tokenType )
	{
		case TOKEN_NUMERIC_LITERAL:
		{
			if ( !AddNumericArgument(context, instructionCWD, argIndex) )
			{
				// Error will have been set.
				return NULL;
			}

			break;
		}

		case TOKEN_LABEL_REFERENCE:
		{
			if ( !AddLabelRefArgument(context, instructionCWD, argIndex) )
			{
				// Error will have been set.
				return NULL;
			}

			break;
		}

		default:
		{
			V2MPAsm_ParseContext_TerminateWithError(
				context,
				PARSEERROR_UNEXPECTED_TOKEN,
				"Expected numeric literal as argument to instruction, but got \"%s\".",
				begin
			);

			return NULL;
		}
	}

	return end;
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
		PARSECONTEXT_INTERNAL_ERROR(
			parser->context,
			"Instruction parsing routines triggered when the parser was in an incorrect state."
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
			endOfToken = ProcessInstructionArgument(parser->context, argsProcessed);
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
				PARSECONTEXT_INTERNAL_ERROR(
					parser->context,
					"Could not determine end of token."
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
			PARSEERROR_INCORRECT_ARGS,
			"Expected %zu %s for \"%s\" instruction, but was given %zu %s.",
			totalArgs,
			totalArgs == 1 ? "argument" : "arguments",
			V2MPAsm_InstructionMeta_GetName(instructionMeta),
			argsProcessed,
			argsProcessed == 1 ? "argument" : "arguments"
		);

		return;
	}

	// Finished with the instruction, so return to a default state.
	V2MPAsm_ParseContext_SetParseState(parser->context, PARSESTATE_DEFAULT);
}
