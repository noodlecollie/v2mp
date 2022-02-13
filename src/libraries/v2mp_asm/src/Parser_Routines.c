#include "Parser_Routines.h"

bool V2MPAsm_Parser_ExecuteParse(V2MPAsm_Parser* parser)
{
	if ( !parser )
	{
		return false;
	}

	switch ( V2MPAsm_ParseContext_GetParseState(parser->context) )
	{
		case PARSESTATE_DEFAULT:
		{
			// TODO
			break;
		}

		default:
		{
			// TODO
			V2MPAsm_ParseContext_SetParseState(parser->context, PARSESTATE_TERMINATED);
			break;
		}
	}

	return false;
}
