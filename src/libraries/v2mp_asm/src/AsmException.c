#include "V2MPAsm/AsmException.h"
#include "V2MPInternal/Util/Heap.h"
#include "AsmException_Internal.h"

size_t V2MPAsm_AsmException_GetLine(const V2MPAsm_AsmException* ex)
{
	return ex ? ex->line : 0;
}

size_t V2MPAsm_AsmException_GetColumn(const V2MPAsm_AsmException* ex)
{
	return ex ? ex->column : 0;
}

V2MPAsm_AsmExceptionType V2MPAsm_AsmException_GetExceptionType(const V2MPAsm_AsmException* ex)
{
	return ex ? ex->exceptionType : V2MPASM_EXCEPTION_NONE;
}

bool V2MPAsm_AsmException_IsValid(const V2MPAsm_AsmException* ex)
{
	return ex ? ex->exceptionType != V2MPASM_EXCEPTION_NONE : false;
}

V2MPAsm_AsmWarningType V2MPAsm_AsmException_GetWarningType(const V2MPAsm_AsmException* ex)
{
	if ( !ex || ex->exceptionType != V2MPASM_EXCEPTION_WARNING )
	{
		return V2MPASM_WARNING_UNSPECIFIED;
	}

	return (V2MPAsm_AsmWarningType)ex->warningOrErrorType;
}

V2MPAsm_AsmErrorType V2MPAsm_AsmException_GetErrorType(const V2MPAsm_AsmException* ex)
{
	if ( !ex || ex->exceptionType != V2MPASM_EXCEPTION_ERROR )
	{
		return V2MPASM_ERROR_UNSPECIFIED;
	}

	return (V2MPAsm_AsmErrorType)ex->warningOrErrorType;
}
