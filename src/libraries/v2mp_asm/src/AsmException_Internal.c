#include "V2MPInternal/Util/Heap.h"
#include "V2MPInternal/Util/String.h"
#include "AsmException_Internal.h"

V2MPAsm_AsmException* V2MPAsm_AsmException_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MPAsm_AsmException);
}

void V2MPAsm_AsmException_DeinitAndFree(V2MPAsm_AsmException* ex)
{
	if ( !ex )
	{
		return;
	}

	V2MPAsm_AsmException_SetTokenName(ex, NULL);

	V2MP_FREE(ex);
}

void V2MPAsm_AsmException_SetTokenName(V2MPAsm_AsmException* ex, const char* tokenName)
{
	if ( !ex )
	{
		return;
	}

	if ( ex->tokenName )
	{
		V2MP_FREE(ex->tokenName);
		ex->tokenName = NULL;
	}

	if ( !tokenName || !(*tokenName) )
	{
		return;
	}

	ex->tokenName = V2MP_String_Duplicate(tokenName);
}

void V2MPAsm_AsmException_SetLine(V2MPAsm_AsmException* ex, size_t line)
{
	if ( !ex )
	{
		return;
	}

	ex->line = line;
}

void V2MPAsm_AsmException_SetColumn(V2MPAsm_AsmException* ex, size_t column)
{
	if ( !ex )
	{
		return;
	}

	ex->column = column;
}

void V2MPAsm_AsmException_SetWarning(V2MPAsm_AsmException* ex, V2MPAsm_AsmWarningType warning)
{
	if ( !ex )
	{
		return;
	}

	ex->exceptionType = V2MPASM_EXCEPTION_WARNING;
	ex->warningOrErrorType = warning;
}

void V2MPAsm_AsmException_SetError(V2MPAsm_AsmException* ex, V2MPAsm_AsmErrorType error)
{
	if ( !ex )
	{
		return;
	}

	ex->exceptionType = V2MPASM_EXCEPTION_ERROR;
	ex->warningOrErrorType = error;
}
