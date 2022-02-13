#include "V2MPAsm/ParseException.h"
#include "ParseException_Internal.h"

V2MPAsm_ParseException_Type V2MPAsm_ParseException_GetType(const V2MPAsm_ParseException* exception)
{
	return exception ? exception->type : V2MPASM_PARSEEXCEPTION_ERROR;
}

void V2MPAsm_ParseException_ToString(const V2MPAsm_ParseException* exception, char* buffer, size_t length)
{
	// TODO
	(void)exception;
	(void)buffer;
	(void)length;
}
