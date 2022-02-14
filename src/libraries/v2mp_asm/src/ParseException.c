#include <stdio.h>
#include <string.h>
#include "V2MPAsm/ParseException.h"
#include "ParseException_Internal.h"
#include "ParseContext.h"

V2MPAsm_ParseException_Type V2MPAsm_ParseException_GetType(const V2MPAsm_ParseException* exception)
{
	return exception ? exception->type : V2MPASM_PARSEEXCEPTION_ERROR;
}

void V2MPAsm_ParseException_ToString(const V2MPAsm_ParseException* exception, char* buffer, size_t length)
{
	const char* filePath = NULL;
	const char* warningOrErrorString;
	int printfResult;

	if ( !buffer || length < 1 )
	{
		return;
	}

	buffer[0] = '\0';

	if ( !exception )
	{
		return;
	}

	if ( exception->context )
	{
		filePath = V2MPAsm_ParseContext_GetFilePath(exception->context);
	}

	warningOrErrorString = V2MPAsm_ParseException_GetWarningOrErrorString(exception);

	// Should never happen
	if ( !warningOrErrorString )
	{
		warningOrErrorString = "UNKNOWN";
	}

	// file_path:line:column warning/error: desc
	printfResult = snprintf(
		buffer,
		length,
		"%s%s%zu:%zu %s: %s",
		filePath ? filePath : "",
		filePath ? ":" : "",
		exception->line,
		exception->column,
		exception->type == V2MPASM_PARSEEXCEPTION_WARNING ? "warning" : "error",
		warningOrErrorString
	);

	buffer[length - 1] = '\0';

	if ( printfResult < 0 )
	{
		// Something went wrong.
		buffer[0] = '\0';
		return;
	}

	if ( !exception->customDescription || (size_t)printfResult >= length )
	{
		return;
	}

	buffer += (size_t)printfResult;
	length -= (size_t)printfResult;

	snprintf(buffer, length, " %s", exception->customDescription);
	buffer[length] = '\0';
}
