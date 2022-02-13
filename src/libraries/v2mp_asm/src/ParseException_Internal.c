#include "ParseException_Internal.h"
#include "BaseUtil/Heap.h"

V2MPAsm_ParseException* V2MPAsm_ParseException_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPAsm_ParseException);
}

void V2MPAsm_ParseException_DeallocateAndFree(V2MPAsm_ParseException* exception)
{
	if ( !exception )
	{
		return;
	}

	BASEUTIL_FREE(exception);
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
