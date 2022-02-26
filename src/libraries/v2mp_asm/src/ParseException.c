#include <stdio.h>
#include <string.h>
#include "V2MPAsm/ParseException.h"
#include "ParseException_Internal.h"
#include "ParseContext.h"
#include "BaseUtil/Util.h"

V2MPAsm_ParseException_Type V2MPAsm_ParseException_GetType(const V2MPAsm_ParseException* exception)
{
	return exception ? exception->type : V2MPASM_PARSEEXCEPTION_ERROR;
}

size_t V2MPAsm_ParseException_ToString(const V2MPAsm_ParseException* exception, char* buffer, size_t length)
{
	const char* filePath = NULL;
	const char* descString;
	int printfResult;

	if ( !buffer || length < 1 )
	{
		return 0;
	}

	buffer[0] = '\0';

	if ( !exception )
	{
		return 0;
	}

	if ( exception->context )
	{
		filePath = V2MPAsm_ParseContext_GetFilePath(exception->context);
	}

	descString = V2MPAsm_ParseException_GetCustomDescription(exception);

	if ( !descString || !(*descString) )
	{
		descString = V2MPAsm_ParseException_GetWarningOrErrorString(exception);

		// Should never happen
		if ( !descString )
		{
			descString = "UNKNOWN";
		}
	}

	// file_path:line:column warning/error: desc [-Werror=exception-id]
	printfResult = snprintf(
		buffer,
		length,
		"%s%s%zu:%zu %s: %s [-W%s=%s]",
		filePath ? filePath : "",
		filePath ? ":" : "",
		exception->line,
		exception->column,
		exception->type == V2MPASM_PARSEEXCEPTION_WARNING ? "warning" : "error",
		descString,
		exception->type == V2MPASM_PARSEEXCEPTION_WARNING ? "warning" : "error",
		V2MPAsm_ParseException_GetWarningOrErrorID(exception)
	);

	if ( printfResult < 0 )
	{
		// Something went wrong.
		buffer[0] = '\0';
		return 0;
	}

	// Just in case:
	buffer[length - 1] = '\0';
	return (size_t)printfResult;
}
