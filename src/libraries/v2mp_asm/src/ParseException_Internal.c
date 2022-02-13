#include "ParseException_Internal.h"
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"

V2MPAsm_ParseException* V2MPAsm_ParseException_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPAsm_ParseException);
}

void V2MPAsm_ParseException_DeinitAndFree(V2MPAsm_ParseException* exception)
{
	if ( !exception )
	{
		return;
	}

	if ( exception->customDescription )
	{
		BASEUTIL_FREE(exception->customDescription);
		exception->customDescription = NULL;
	}

	BASEUTIL_FREE(exception);
}

struct V2MPAsm_ParseContext* V2MPAsm_ParseException_GetContext(const V2MPAsm_ParseException* exception)
{
	return exception ? exception->context : NULL;
}

void V2MPAsm_ParseException_SetContext(V2MPAsm_ParseException* exception, struct V2MPAsm_ParseContext* context)
{
	if ( !exception )
	{
		return;
	}

	exception->context = context;
}

void V2MPAsm_ParseException_SetWarning(V2MPAsm_ParseException* exception, V2MPAsm_ParseWarningType type)
{
	if ( !exception )
	{
		return;
	}

	exception->type = V2MPASM_PARSEEXCEPTION_WARNING;
	exception->warningOrErrorType = type;
}

void V2MPAsm_ParseException_SetError(V2MPAsm_ParseException* exception, V2MPAsm_ParseErrorType type)
{
	if ( !exception )
	{
		return;
	}

	exception->type = V2MPASM_PARSEEXCEPTION_ERROR;
	exception->warningOrErrorType = type;
}

const char* V2MPAsm_ParseError_GetCustomDescription(const V2MPAsm_ParseException* exception)
{
	return exception ? exception->customDescription : NULL;
}

void V2MPAsm_ParseException_SetCustomDescription(V2MPAsm_ParseException* exception, const char* description)
{
	if ( !exception )
	{
		return;
	}

	if ( exception->customDescription )
	{
		BASEUTIL_FREE(exception->customDescription);
		exception->customDescription = NULL;
	}

	if ( !description )
	{
		return;
	}

	exception->customDescription = BaseUtil_String_Duplicate(description);
}

size_t V2MPAsm_ParseException_GetLine(const V2MPAsm_ParseException* exception)
{
	return exception ? exception->line : 0;
}

size_t V2MPAsm_ParseException_GetColumn(const V2MPAsm_ParseException* exception)
{
	return exception ? exception->column : 0;
}

void V2MPAsm_ParseException_SetLineAndColumn(V2MPAsm_ParseException* exception, size_t line, size_t column)
{
	if ( !exception )
	{
		return;
	}

	exception->line = line;
	exception->column = column;
}

void V2MPAsm_ParseException_ToStringInternal(
	const V2MPAsm_ParseException* exception,
	const char* filePath,
	char* buffer,
	size_t length
)
{
	// TODO
	(void)exception;
	(void)filePath;
	(void)buffer;
	(void)length;
}
