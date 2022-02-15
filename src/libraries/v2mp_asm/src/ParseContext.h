#ifndef V2MPASM_PARSECONTEXT_H
#define V2MPASM_PARSECONTEXT_H

#include <stdbool.h>
#include <stdarg.h>
#include "V2MPAsm/ParseException.h"
#include "SharedComponents/DoubleLinkedList.h"
#include "BaseUtil/Filesystem.h"
#include "InputFile.h"
#include "ParseException_Internal.h"
#include "Tokens/TokenMeta.h"

typedef enum V2MPAsm_ParseState
{
	PARSESTATE_DEFAULT = 0,
	PARSESTATE_TERMINATED
} V2MPAsm_ParseState;

typedef struct V2MPAsm_ParseContext
{
	V2MPAsm_InputFile* inputFile;
	char* filePath;
	const char* fileName;

	V2MPAsm_ParseState state;

	char* currentTokenBuffer;
	size_t currentTokenBufferSize;
	size_t currentTokenLength;
	V2MPAsm_TokenContext currentTokenContext;

	V2MPSC_DoubleLL* exceptionsList;
} V2MPAsm_ParseContext;

typedef struct V2MPAsm_ParseContext_ExceptionNode
{
	V2MPSC_DoubleLL_Node* listNode;
	V2MPAsm_ParseException* exception;
} V2MPAsm_ParseContext_ExceptionNode;

V2MPAsm_ParseContext* V2MPAsm_ParseContext_AllocateAndInit(void);
void V2MPAsm_ParseContext_DeinitAndFree(V2MPAsm_ParseContext* context);

V2MPAsm_InputFile* V2MPAsm_ParseContext_GetInputFile(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetFilePath(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetFileName(const V2MPAsm_ParseContext* context);

bool V2MPAsm_ParseContext_SetInput(V2MPAsm_ParseContext* context, const char* filePath, const char* data, size_t length);
bool V2MPAsm_ParseContext_HasInput(const V2MPAsm_ParseContext* context);
bool V2MPAsm_ParseContext_InputIsAtEOF(const V2MPAsm_ParseContext* context);
size_t V2MPAsm_ParseContext_GetInputLineNumber(const V2MPAsm_ParseContext* context);
size_t V2MPAsm_ParseContext_GetInputColumnNumber(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetInputCursor(const V2MPAsm_ParseContext* context);

void V2MPAsm_ParseContext_SeekInput(V2MPAsm_ParseContext* context, const char* pos);
const char* V2MPAsm_ParseContext_GetBeginningOfNextToken(V2MPAsm_ParseContext* context);

V2MPAsm_ParseState V2MPAsm_ParseContext_GetParseState(const V2MPAsm_ParseContext* context);
void V2MPAsm_ParseContext_SetParseState(V2MPAsm_ParseContext* context, V2MPAsm_ParseState state);

V2MPAsm_TokenContext V2MPAsm_ParseContext_GetTokenContext(const V2MPAsm_ParseContext* context);
void V2MPAsm_ParseContext_SetTokenContext(V2MPAsm_ParseContext* context, V2MPAsm_TokenContext tokenContext);

// Token length does not include the terminator.
bool V2MPAsm_ParseContext_SetCurrentToken(V2MPAsm_ParseContext* context, const char* begin, const char* end);
bool V2MPAsm_ParseContext_SetCurrentTokenFromInput(V2MPAsm_ParseContext* context, V2MPAsm_TokenType tokenType);
const char* V2MPAsm_ParseContext_GetCurrentToken(const V2MPAsm_ParseContext* context);
size_t V2MPAsm_ParseContext_GetCurrentTokenLength(const V2MPAsm_ParseContext* context);

size_t V2MPAsm_ParseContext_GetExceptionCount(const V2MPAsm_ParseContext* context);
V2MPAsm_ParseContext_ExceptionNode* V2MPAsm_ParseContext_AppendException(V2MPAsm_ParseContext* context);
V2MPAsm_ParseContext_ExceptionNode* V2MPAsm_ParseContext_GetFirstException(const V2MPAsm_ParseContext* context);
V2MPAsm_ParseContext_ExceptionNode* V2MPAsm_ParseContext_GetNextException(const V2MPAsm_ParseContext_ExceptionNode* node);

void V2MPAsm_ParseContext_SetExceptionLocationFromContext(const V2MPAsm_ParseContext* context, V2MPAsm_ParseException* exception);

void V2MPAsm_ParseContext_CreateAndSetWarning(V2MPAsm_ParseContext* context, V2MPAsm_ParseWarningType warningType, const char* format, ...);
void V2MPAsm_ParseContext_CreateAndSetWarningV(V2MPAsm_ParseContext* context, V2MPAsm_ParseWarningType warningType, const char* format, va_list args);

void V2MPAsm_ParseContext_CreateAndSetError(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, ...);
void V2MPAsm_ParseContext_CreateAndSetErrorV(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, va_list args);

void V2MPAsm_ParseContext_TerminateWithError(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, ...);
void V2MPAsm_ParseContext_TerminateWithErrorV(V2MPAsm_ParseContext* context, V2MPAsm_ParseErrorType errorType, const char* format, va_list args);

#endif // V2MPASM_PARSECONTEXT_H
