#include "V2MPAsm/AsmException.h"

V2MPAsm_AsmException* V2MPAsm_AsmException_AllocateAndInit(void)
{
	// TODO
	return NULL;
}

void V2MPAsm_AsmException_DeinitAndFree(V2MPAsm_AsmException* ex)
{
	// TODO
	(void)ex;
}

size_t V2MPAsm_AsmException_GetLine(const V2MPAsm_AsmException* ex)
{
	// TODO
	(void)ex;
	return 0;
}

size_t V2MPAsm_AsmException_GetColumn(const V2MPAsm_AsmException* ex)
{
	// TODO
	(void)ex;
	return 0;
}

V2MPAsm_AsmExceptionType V2MPAsm_AsmException_GetExceptionType(const V2MPAsm_AsmException* ex)
{
	// TODO
	(void)ex;
	return V2MPASM_EXCEPTION_NONE;
}

V2MPAsm_AsmWarningType V2MPAsm_AsmException_GetWarningType(const V2MPAsm_AsmException* ex)
{
	// TODO
	(void)ex;
	return V2MPASM_WARNING_NONE;
}

V2MPAsm_AsmErrorType V2MPAsm_AsmException_GetErrorType(const V2MPAsm_AsmException* ex)
{
	// TODO
	(void)ex;
	return V2MPASM_ERROR_NONE;
}
