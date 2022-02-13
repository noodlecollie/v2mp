#include "Parser_Routines.h"
#include "ParseException_Internal.h"

static bool ParseDefault(V2MPAsm_ParseContext* context)
{
	V2MPAsm_ParseContext_ExceptionNode* node;

	node = V2MPAsm_ParseContext_AppendException(context);

	if ( node )
	{
		V2MPAsm_ParseException_SetError(node->exception, PARSEERROR_INTERNAL);
		V2MPAsm_ParseException_SetCustomDescription(node->exception, "ParseDefault() routine has not been implemented yet.");
	}

	V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
	return true;
}

bool V2MPAsm_Parser_ExecuteParse(V2MPAsm_Parser* parser)
{
	bool success = false;

	if ( !parser )
	{
		return false;
	}

	switch ( V2MPAsm_ParseContext_GetParseState(parser->context) )
	{
		case PARSESTATE_DEFAULT:
		{
			success = ParseDefault(parser->context);
			break;
		}

		default:
		{
			V2MPAsm_ParseContext_ExceptionNode* node;

			node = V2MPAsm_ParseContext_AppendException(parser->context);

			if ( node )
			{
				V2MPAsm_ParseException_SetError(node->exception, PARSEERROR_INTERNAL);
				V2MPAsm_ParseException_SetCustomDescription(node->exception, "Unknown internal state encountered.");
			}

			V2MPAsm_ParseContext_SetParseState(parser->context, PARSESTATE_TERMINATED);
			break;
		}
	}

	return success;
}
