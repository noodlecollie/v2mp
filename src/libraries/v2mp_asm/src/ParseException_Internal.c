#include <stdio.h>
#include "ParseException_Internal.h"
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "BaseUtil/Util.h"

const char* V2MPAsm_ParseException_GetWarningString(V2MPAsm_ParseWarningType warning)
{
#define LIST_ITEM(value, id, desc) desc,
	static const char* const STRINGS[] =
	{
		V2MPASM_PARSEWARNINGTYPE_LIST
	};
#undef LIST_ITEM

	return (size_t)warning < BASEUTIL_ARRAY_SIZE(STRINGS)
		? STRINGS[(size_t)warning]
		: "UNKNOWN";
}

const char* V2MPAsm_ParseException_GetWarningID(V2MPAsm_ParseWarningType warning)
{
#define LIST_ITEM(value, id, desc) id,
	static const char* const STRINGS[] =
	{
		V2MPASM_PARSEWARNINGTYPE_LIST
	};
#undef LIST_ITEM

	return (size_t)warning < BASEUTIL_ARRAY_SIZE(STRINGS)
		? STRINGS[(size_t)warning]
		: "UNKNOWN";
}

const char* V2MPAsm_ParseException_GetErrorString(V2MPAsm_ParseErrorType error)
{
#define LIST_ITEM(value, id, desc) desc,
	static const char* const STRINGS[] =
	{
		V2MPASM_PARSEERRORTYPE_LIST
	};
#undef LIST_ITEM

	return (size_t)error < BASEUTIL_ARRAY_SIZE(STRINGS)
		? STRINGS[(size_t)error]
		: "UNKNOWN";
}

const char* V2MPAsm_ParseException_GetErrorID(V2MPAsm_ParseErrorType error)
{
#define LIST_ITEM(value, id, desc) id,
	static const char* const STRINGS[] =
	{
		V2MPASM_PARSEERRORTYPE_LIST
	};
#undef LIST_ITEM

	return (size_t)error < BASEUTIL_ARRAY_SIZE(STRINGS)
		? STRINGS[(size_t)error]
		: "UNKNOWN";
}

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

const char* V2MPAsm_ParseException_GetWarningOrErrorString(const V2MPAsm_ParseException* exception)
{
	if ( !exception )
	{
		return NULL;
	}

	if ( exception->type == V2MPASM_PARSEEXCEPTION_WARNING )
	{
		return V2MPAsm_ParseException_GetWarningString((V2MPAsm_ParseWarningType)exception->warningOrErrorType);
	}
	else
	{
		return V2MPAsm_ParseException_GetErrorString((V2MPAsm_ParseErrorType)exception->warningOrErrorType);
	}
}

const char* V2MPAsm_ParseException_GetWarningOrErrorID(const V2MPAsm_ParseException* exception)
{
	if ( !exception )
	{
		return NULL;
	}

	if ( exception->type == V2MPASM_PARSEEXCEPTION_WARNING )
	{
		return V2MPAsm_ParseException_GetWarningID((V2MPAsm_ParseWarningType)exception->warningOrErrorType);
	}
	else
	{
		return V2MPAsm_ParseException_GetErrorID((V2MPAsm_ParseErrorType)exception->warningOrErrorType);
	}
}

const char* V2MPAsm_ParseException_GetCustomDescription(const V2MPAsm_ParseException* exception)
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

void V2MPAsm_ParseException_FormatCustomDescription(V2MPAsm_ParseException* exception, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	V2MPAsm_ParseException_FormatCustomDescriptionV(exception, format, list);
	va_end(list);
}

void V2MPAsm_ParseException_FormatCustomDescriptionV(V2MPAsm_ParseException* exception, const char* format, va_list args)
{
	char* buffer;

	if ( !exception || !format )
	{
		return;
	}

	V2MPAsm_ParseException_SetCustomDescription(exception, NULL);

	buffer = (char*)BASEUTIL_MALLOC(V2MPASM_PARSEEXCEPTION_FORMAT_DESC_MAX_LENGTH);

	if ( !buffer )
	{
		return;
	}

	buffer[0] = '\0';

	if ( vsnprintf(buffer, V2MPASM_PARSEEXCEPTION_FORMAT_DESC_MAX_LENGTH, format, args) > 0 )
	{
		buffer[V2MPASM_PARSEEXCEPTION_FORMAT_DESC_MAX_LENGTH - 1] = '\0';
	}
	else
	{
		buffer[0] = '\0';
	}

	V2MPAsm_ParseException_SetCustomDescription(exception, buffer);
	BASEUTIL_FREE(buffer);
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
