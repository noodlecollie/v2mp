#include "V2MPAsm/Parser.h"
#include "BaseUtil/Heap.h"
#include "Parser_Internal.h"
#include "ParseContext.h"
#include "Parser_Routines.h"

V2MPAsm_Parser* V2MPAsm_Parser_AllocateAndInit(void)
{
	V2MPAsm_Parser* parser = BASEUTIL_CALLOC_STRUCT(V2MPAsm_Parser);

	if ( !parser )
	{
		return NULL;
	}

	parser->context = V2MPAsm_ParseContext_AllocateAndInit();

	return parser;
}

void V2MPAsm_Parser_DeinitAndFree(V2MPAsm_Parser* parser)
{
	if ( !parser )
	{
		return;
	}

	V2MPAsm_ParseContext_DeinitAndFree(parser->context);

	BASEUTIL_FREE(parser);
}

void V2MPAsm_Parser_SetInput(V2MPAsm_Parser* parser, const char* filePath, const char* data, size_t dataLength)
{
	if ( !parser )
	{
		return;
	}

	V2MPAsm_ParseContext_SetInput(parser->context, filePath, data, dataLength);
}

bool V2MPAsm_Parser_HasInput(const V2MPAsm_Parser* parser)
{
	return parser && V2MPAsm_ParseContext_HasInput(parser->context);
}

bool V2MPAsm_Parser_Parse(V2MPAsm_Parser* parser)
{
	if ( !parser || !V2MPAsm_Parser_HasInput(parser) )
	{
		return false;
	}

	return V2MPAsm_Parser_ExecuteParse(parser);
}
