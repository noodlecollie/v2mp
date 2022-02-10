#include <string.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "ParseContext.h"
#include "cwalk/cwalk.h"

static inline void FreeFilePath(V2MPAsm_ParseContext* context)
{
	if ( context->filePath )
	{
		BASEUTIL_FREE(context->filePath);
	}

	context->filePath = NULL;
	context->fileName = NULL;
}

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

	V2MPAsm_ParseContext_AllocateLineBuffer(context, 0);
	FreeFilePath(context);
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

bool V2MPAsm_ParseContext_SetInput(V2MPAsm_ParseContext* context, const char* filePath, const V2MPAsm_Byte* data, size_t length)
{
	if ( !context )
	{
		return false;
	}

	FreeFilePath(context);

	if ( filePath )
	{
		context->filePath = BaseUtil_String_Duplicate(filePath);

		if ( !context->filePath )
		{
			return false;
		}

		cwk_path_get_basename(context->filePath, &context->fileName, NULL);
	}

	V2MPAsm_InputFile_SetInput(context->inputFile, data, length);
	return true;
}

bool V2MPAsm_ParseContext_HasInput(const V2MPAsm_ParseContext* context)
{
	return context && V2MPAsm_InputFile_IsValid(context->inputFile);
}

bool V2MPAsm_ParseContext_AllocateLineBuffer(V2MPAsm_ParseContext* context, size_t bufferSize)
{
	if ( !context )
	{
		return false;
	}

	if ( context->lineBuffer )
	{
		BASEUTIL_FREE(context->lineBuffer);
		context->lineBuffer = NULL;
		context->lineBufferSize = 0;
	}

	if ( bufferSize < 1 )
	{
		return true;
	}

	context->lineBuffer = (char*)BASEUTIL_MALLOC(bufferSize);

	if ( !context->lineBuffer )
	{
		return false;
	}

	context->lineBufferSize = bufferSize;
	return true;
}
