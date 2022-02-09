#include <string.h>
#include "BaseUtil/Heap.h"
#include "ParseContext.h"
#include "cwalk/cwalk.h"

static inline void ResetFilePath(V2MPAsm_ParseContext* context)
{
	context->filePath[0] = '\0';
	context->fileName = context->filePath;
}

V2MPAsm_ParseContext* V2MPAsm_ParseContext_AllocateAndInit(void)
{
	V2MPAsm_ParseContext* context = BASEUTIL_MALLOC_STRUCT(V2MPAsm_ParseContext);

	context->inputFile = V2MPAsm_InputFile_AllocateAndInit();
	ResetFilePath(context);

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

V2MPAsm_InputFile* V2MPAsm_ParseContext_GetInputFile(const V2MPAsm_ParseContext* context)
{
	return context ? context->inputFile : NULL;
}

const char* V2MPAsm_ParseContext_GetFilePath(const V2MPAsm_ParseContext* context)
{
	return context ? context->filePath : NULL;
}

const char* V2MPAsm_ParseContext_GetFileName(const V2MPAsm_ParseContext* context)
{
	return context ? context->fileName : NULL;
}

void V2MPAsm_ParseContext_SetInput(V2MPAsm_ParseContext* context, const char* filePath, const V2MPAsm_Byte* data, size_t length)
{
	if ( !context )
	{
		return;
	}

	if ( filePath )
	{
		size_t length = 0;

		strncpy(context->filePath, filePath, sizeof(context->filePath));
		context->filePath[sizeof(context->filePath) - 1] = '\0';

		// TODO: Make a cwalk PR so that the length is optional!
		cwk_path_get_basename(context->filePath, &context->fileName, &length);
	}
	else
	{
		ResetFilePath(context);
	}

	V2MPAsm_InputFile_SetInput(context->inputFile, data, length);
}

bool V2MPAsm_ParseContext_HasInput(const V2MPAsm_ParseContext* context)
{
	return context && V2MPAsm_InputFile_IsValid(context->inputFile);
}
