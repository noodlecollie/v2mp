#include <string.h>
#include "BaseUtil/Heap.h"
#include "ParseContext.h"

V2MPAsm_ParseContext* V2MPAsm_ParseContext_AllocateAndInit(void)
{
	V2MPAsm_ParseContext* context = BASEUTIL_CALLOC_STRUCT(V2MPAsm_ParseContext);

	context->inputFile = V2MPAsm_InputFile_AllocateAndInit();

	return context;
}

void V2MPAsm_ParseContext_DeinitAndFree(V2MPAsm_ParseContext* context)
{
	if ( !context )
	{
		return;
	}

	V2MPAsm_InputFile_DeinitAndFree(context->inputFile);

	BASEUTIL_FREE(context);
}

void V2MPAsm_ParseContext_SetInput(V2MPAsm_ParseContext* context, const char* filePath, const V2MPAsm_Byte* data, size_t length)
{
	if ( !context )
	{
		return;
	}

	strncpy(context->filePath, filePath, sizeof(context->filePath));
	context->filePath[sizeof(context->filePath) - 1] = '\0';

	// TODO: Set file name pointer
	// We could probably do with https://github.com/likle/cwalk

	V2MPAsm_InputFile_SetInput(context->inputFile, data, length);
}

bool V2MPAsm_ParseContext_HasInput(const V2MPAsm_ParseContext* context)
{
	return context && V2MPAsm_InputFile_IsValid(context->inputFile);
}
