#include <string.h>
#include <stdio.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "ParseContext.h"
#include "cwalk/cwalk.h"
#include "ParseException_Internal.h"

#define V2MPASM_PARSECONTEXT_EX_DESC_LENGTH 512

static void DestroyParseExceptionNode(void* payload)
{
	V2MPAsm_ParseException_DeinitAndFree(((V2MPAsm_ParseContext_ExceptionNode*)payload)->exception);
}

static inline void FreeFilePath(V2MPAsm_ParseContext* context)
{
	if ( context->filePath )
	{
		BASEUTIL_FREE(context->filePath);
	}

	context->filePath = NULL;
	context->fileName = NULL;
}

static void FreeCurrentToken(V2MPAsm_ParseContext* context)
{
	if ( context->currentTokenBuffer )
	{
		BASEUTIL_FREE(context->currentTokenBuffer);
		context->currentTokenBuffer = NULL;
	}

	context->currentTokenBufferSize = 0;
	context->currentTokenLength = 0;
}

void CreateAndSetExceptionV(
	V2MPAsm_ParseContext* context,
	bool isError,
	int exceptionSubtype,
	const char* format,
	va_list args
)
{
	char* buffer;
	V2MPAsm_ParseContext_ExceptionNode* node;

	if ( !context || !format )
	{
		return;
	}

	node = V2MPAsm_ParseContext_AppendException(context);

	if ( !node )
	{
		return;
	}

	buffer = (char*)BASEUTIL_MALLOC(V2MPASM_PARSECONTEXT_EX_DESC_LENGTH);

	if ( buffer )
	{
		buffer[0] = '\0';

		vsnprintf(buffer, V2MPASM_PARSECONTEXT_EX_DESC_LENGTH, format, args);
		buffer[V2MPASM_PARSECONTEXT_EX_DESC_LENGTH - 1] = '\0';
	}
	else
	{
		static char dummyBuffer[] = { '\0' };
		buffer = dummyBuffer;
	}

	if ( isError )
	{
		V2MPAsm_ParseException_SetError(node->exception, (V2MPAsm_ParseErrorType)exceptionSubtype);
	}
	else
	{
		V2MPAsm_ParseException_SetWarning(node->exception, (V2MPAsm_ParseWarningType)exceptionSubtype);
	}

	V2MPAsm_ParseException_SetCustomDescription(node->exception, buffer);

	V2MPAsm_ParseException_SetLineAndColumn(
		node->exception,
		V2MPAsm_ParseContext_GetInputLineNumber(context),
		V2MPAsm_ParseContext_GetInputColumnNumber(context)
	);
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

		context->exceptionsList = V2MPSC_DoubleLL_AllocateAndInit(sizeof(V2MPAsm_ParseContext_ExceptionNode), &DestroyParseExceptionNode);

		if ( !context->exceptionsList )
		{
			break;
		}

		context->cwdList = V2MPAsm_CWDList_AllocateAndInit();

		if ( !context->cwdList )
		{
			break;
		}

		return context;
	}
	while ( false );

	if ( context )
	{
		V2MPAsm_ParseContext_DeinitAndFree(context);
	}

	return NULL;
}

void V2MPAsm_ParseContext_DeinitAndFree(V2MPAsm_ParseContext* context)
{
	if ( !context )
	{
		return;
	}

	V2MPAsm_CWDList_DeinitAndFree(context->cwdList);
	context->cwdList = NULL;

	V2MPSC_DoubleLL_DeinitAndFree(context->exceptionsList);
	context->exceptionsList = NULL;

	FreeFilePath(context);
	FreeCurrentToken(context);

	V2MPAsm_InputFile_DeinitAndFree(context->inputFile);
	context->inputFile = NULL;

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

void V2MPAsm_ParseContext_SeekInputToFirstTokenOnNextLine(V2MPAsm_ParseContext* context)
{
	if ( !context )
	{
		return;
	}

	V2MPAsm_InputFile_SkipToNextLine(context->inputFile);
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

bool V2MPAsm_ParseContext_SetCurrentToken(V2MPAsm_ParseContext* context, const char* begin, const char* end)
{
	size_t length;

	if ( !context || (!begin && end) || (!end && begin) )
	{
		return false;
	}

	if ( !begin || !end || end < begin )
	{
		FreeCurrentToken(context);
		return true;
	}

	length = end - begin;

	if ( length >= context->currentTokenBufferSize )
	{
		char* newBuffer;

		FreeCurrentToken(context);

		newBuffer = (char*)BASEUTIL_MALLOC(length + 1);

		if ( !newBuffer )
		{
			return false;
		}

		context->currentTokenBuffer = newBuffer;
		context->currentTokenBufferSize = length + 1;
	}

	context->currentTokenLength = length;

	if ( context->currentTokenLength > 0 )
	{
		memcpy(context->currentTokenBuffer, begin, context->currentTokenLength);
	}

	context->currentTokenBuffer[context->currentTokenLength] = '\0';

	return true;
}

bool V2MPAsm_ParseContext_SetCurrentTokenFromInput(V2MPAsm_ParseContext* context, V2MPAsm_TokenType tokenType)
{
	const char* begin;
	const V2MPAsm_TokenMeta* tokenMeta;

	if ( !context )
	{
		return false;
	}

	begin = V2MPAsm_ParseContext_GetInputCursor(context);
	tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);

	return V2MPAsm_ParseContext_SetCurrentToken(
		context,
		begin,
		V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, begin)
	);
}

const char* V2MPAsm_ParseContext_GetCurrentToken(const V2MPAsm_ParseContext* context)
{
	return context ? context->currentTokenBuffer : NULL;
}

size_t V2MPAsm_ParseContext_GetCurrentTokenLength(const V2MPAsm_ParseContext* context)
{
	return context ? context->currentTokenLength : 0;
}

V2MPAsm_CWDBase* V2MPAsm_ParseContext_AppendNewCWDAsCurrent(V2MPAsm_ParseContext* context, V2MPAsm_CWD_Type cwdType)
{
	if ( !context )
	{
		return NULL;
	}

	context->currentCWD = V2MPAsm_CWDList_AppendNew(context->cwdList, cwdType);
	return context->currentCWD;
}

V2MPAsm_CWDBase* V2MPAsm_ParseContext_GetCurrentCWD(const V2MPAsm_ParseContext* context)
{
	return context ? context->currentCWD : NULL;
}

size_t V2MPAsm_ParseContext_GetExceptionCount(const V2MPAsm_ParseContext* context)
{
	if ( !context )
	{
		return 0;
	}

	return V2MPSC_DoubleLL_GetNodeCount(context->exceptionsList);
}

V2MPAsm_ParseContext_ExceptionNode* V2MPAsm_ParseContext_AppendException(V2MPAsm_ParseContext* context)
{
	V2MPSC_DoubleLL_Node* listNode = NULL;

	do
	{
		V2MPAsm_ParseContext_ExceptionNode* exceptionNode;

		if ( !context )
		{
			break;
		}

		listNode = V2MPSC_DoubleLL_AppendToTail(context->exceptionsList);

		if ( !listNode )
		{
			break;
		}

		exceptionNode = (V2MPAsm_ParseContext_ExceptionNode*)V2MPSC_DoubleLLNode_GetPayload(listNode);

		if ( !exceptionNode )
		{
			break;
		}

		exceptionNode->exception = V2MPAsm_ParseException_AllocateAndInit();

		if ( !exceptionNode->exception )
		{
			break;
		}

		exceptionNode->listNode = listNode;
		V2MPAsm_ParseException_SetContext(exceptionNode->exception, context);

		return exceptionNode;
	}
	while ( false );

	if ( listNode )
	{
		V2MPSC_DoubleLLNode_Destroy(listNode);
	}

	return NULL;
}

V2MPAsm_ParseContext_ExceptionNode* V2MPAsm_ParseContext_GetFirstException(const V2MPAsm_ParseContext* context)
{
	V2MPSC_DoubleLL_Node* listNode;

	if ( !context )
	{
		return NULL;
	}

	listNode = V2MPSC_DoubleLL_GetHead(context->exceptionsList);

	return listNode ? (V2MPAsm_ParseContext_ExceptionNode*)V2MPSC_DoubleLLNode_GetPayload(listNode) : NULL;
}

V2MPAsm_ParseContext_ExceptionNode* V2MPAsm_ParseContext_GetNextException(const V2MPAsm_ParseContext_ExceptionNode* node)
{
	V2MPSC_DoubleLL_Node* listNode;

	if ( !node )
	{
		return NULL;
	}

	listNode = V2MPSC_DoubleLLNode_GetNext(node->listNode);

	return listNode ? (V2MPAsm_ParseContext_ExceptionNode*)V2MPSC_DoubleLLNode_GetPayload(listNode) : NULL;
}

void V2MPAsm_ParseContext_SetExceptionLocationFromContext(const V2MPAsm_ParseContext* context, V2MPAsm_ParseException* exception)
{
	if ( !context || !exception )
	{
		return;
	}

	V2MPAsm_ParseException_SetLineAndColumn(
		exception,
		V2MPAsm_ParseContext_GetInputLineNumber(context),
		V2MPAsm_ParseContext_GetInputColumnNumber(context)
	);
}

void V2MPAsm_ParseContext_CreateAndSetWarning(V2MPAsm_ParseContext* context, V2MPAsm_ParseWarningType warningType, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	V2MPAsm_ParseContext_CreateAndSetWarningV(context, warningType, format, list);
	va_end(list);
}

void V2MPAsm_ParseContext_CreateAndSetWarningV(V2MPAsm_ParseContext* context, V2MPAsm_ParseWarningType warningType, const char* format, va_list args)
{
	CreateAndSetExceptionV(context, false, warningType, format, args);
}

void V2MPAsm_ParseContext_CreateAndSetError(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	V2MPAsm_ParseContext_CreateAndSetErrorV(context, errorType, format, list);
	va_end(list);
}

void V2MPAsm_ParseContext_CreateAndSetErrorV(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, va_list args)
{
	if ( !context )
	{
		return;
	}

	CreateAndSetExceptionV(context, true, errorType, format, args);
}

void V2MPAsm_ParseContext_TerminateWithError(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	V2MPAsm_ParseContext_TerminateWithErrorV(context, errorType, format, list);
	va_end(list);
}

void V2MPAsm_ParseContext_TerminateWithErrorV(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, va_list args)
{
	if ( !context )
	{
		return;
	}

	V2MPAsm_ParseContext_CreateAndSetErrorV(context, errorType, format, args);
	V2MPAsm_ParseContext_SetParseState(context, PARSESTATE_TERMINATED);
}
