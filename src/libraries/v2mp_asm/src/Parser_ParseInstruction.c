#include "Parser_ParseInstruction.h"
#include "Instructions/InstructionMeta.h"
#include "ParseException_Internal.h"
#include "CodewordDescriptors/CWD_Instruction.h"

// void CreateInstructionCWDFromCurrentToken(V2MPAsm_ParseContext* context)
// {
// 	const V2MPAsm_InstructionMeta* instructionMeta;
// 	V2MPAsm_CWDInstruction* instructionCWD;

// 	instructionMeta = V2MPAsm_InstructionMeta_FindCommand(
// 		V2MPAsm_ParseContext_GetCurrentToken(context),
// 		V2MPAsm_ParseContext_GetCurrentTokenLength(context)
// 	);

// 	if ( !instructionMeta )
// 	{
// 		V2MPAsm_ParseContext_TerminateWithError(
// 			context,
// 			PARSEERROR_UNRECOGNISED_INSTRUCTION,
// 			"\"%s\" was not recognised as a CPU instruction or an alias.",
// 			V2MPAsm_ParseContext_GetCurrentToken(context)
// 		);

// 		return;
// 	}

// 	instructionCWD = V2MPAsm_CWDInstruction_Cast(
// 		V2MPAsm_ParseContext_AppendNewCWDAsCurrent(context, V2MPASM_CWD_INSTRUCTION)
// 	);

// 	if ( !instructionCWD )
// 	{
// 		V2MPAsm_ParseContext_TerminateWithError(
// 			context,
// 			PARSEERROR_INTERNAL,
// 			"Could not create instruction data for \"%s\"",
// 			V2MPAsm_ParseContext_GetCurrentToken(context)
// 		);

// 		return;
// 	}

// 	V2MPAsm_CWDInstruction_SetInstructionMeta(instructionCWD, instructionMeta);

// 	// We now have an instruction codeword - return so that the next time we come around,
// 	// we can parse the arguments for the codeword.
// }

void V2MPAsm_Parser_ParseInstruction(V2MPAsm_Parser* parser)
{
	// Sanity:
	if ( V2MPAsm_ParseContext_GetParseState(parser->context) != PARSESTATE_BUILDING_INSTRUCTION )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			parser->context,
			PARSEERROR_INTERNAL,
			"Instruction parsing routines triggered when the parser was in an incorrect state."
		);

		return;
	}

	// TODO: Clear the token list, parse everything on the current line,
	// and add all tokens to the list.

	V2MPAsm_ParseContext_TerminateWithError(
		parser->context,
		PARSEERROR_UNIMPLEMENTED,
		"Instruction parsing still to be implemented."
	);
}
