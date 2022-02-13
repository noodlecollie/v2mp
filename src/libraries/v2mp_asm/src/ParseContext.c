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
	V2MPAsm_ParseContext* context;

	do
	{
		context = BASEUTIL_CALLOC_STRUCT(V2MPAsm_ParseContext);

		if ( !context )
		{
			break;
		}

		context->inputFile = V2MPAsm_InputFile_AllocateAndInit();

		if ( !context->inputFile )
		{
			break;
		}

		return context;
	}
	while ( false );

	if ( context )
	{
		BASEUTIL_FREE(context);
	}

	return NULL;
}

void V2MPAsm_ParseContext_DeinitAndFree(V2MPAsm_ParseContext* context)
{
	if ( !context )
	{
		return;
	}

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

bool V2MPAsm_ParseContext_InputIsAtEOF(const V2MPAsm_ParseContext* context)
{
	return context ? V2MPAsm_InputFile_IsEOF(context->inputFile) : true;
}

size_t V2MPAsm_ParseContext_GetInputLineNumber(const V2MPAsm_ParseContext* context)
{
	return context ? V2MPAsm_InputFile_GetCurrentLineNumber(context->inputFile) : 0;
}

size_t V2MPAsm_ParseContext_GetInputColumnNumber(const V2MPAsm_ParseContext* context)
{
	return context ? V2MPAsm_InputFile_GetCurrentColumnNumber(context->inputFile) : 0;
}

const char* V2MPAsm_ParseContext_GetInputCursor(const V2MPAsm_ParseContext* context)
{
	return context ? V2MPAsm_InputFile_GetCursor(context->inputFile) : NULL;
}

bool V2MPAsm_ParseContext_SetInput(V2MPAsm_ParseContext* context, const char* filePath, const char* data, size_t length)
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

void V2MPAsm_ParseContext_SeekInput(V2MPAsm_ParseContext* context, const char* pos)
{
	if ( !context )
	{
		return;
	}

	V2MPAsm_InputFile_SkipToCursor(context->inputFile, pos);
}

const char* V2MPAsm_ParseContext_GetBeginningOfNextToken(V2MPAsm_ParseContext* context)
{
	if ( !context )
	{
		return NULL;
	}

	V2MPAsm_InputFile_SkipWhitespace(context->inputFile);
	return V2MPAsm_InputFile_GetCursor(context->inputFile);
}

V2MPAsm_ParseState V2MPAsm_ParseContext_GetParseState(const V2MPAsm_ParseContext* context)
{
	return context ? context->state : PARSESTATE_DEFAULT;
}

void V2MPAsm_ParseContext_SetParseState(V2MPAsm_ParseContext* context, V2MPAsm_ParseState state)
{
	if ( !context )
	{
		return;
	}

	context->state = state;
}
