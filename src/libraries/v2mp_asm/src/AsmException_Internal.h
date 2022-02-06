#ifndef V2MPASM_ASMEXCEPTION_INTERNAL_H
#define V2MPASM_ASMEXCEPTION_INTERNAL_H

#include <stddef.h>
#include "V2MPAsm/AsmException.h"

struct V2MPAsm_AsmException
{
	size_t line;
	size_t column;
	V2MPAsm_AsmExceptionType exceptionType;
	int warningOrErrorType;
	char* tokenName;
};

V2MPAsm_AsmException* V2MPAsm_AsmException_AllocateAndInit(void);
void V2MPAsm_AsmException_DeinitAndFree(V2MPAsm_AsmException* ex);

void V2MPAsm_AsmException_SetTokenName(V2MPAsm_AsmException* ex, const char* tokenName);
void V2MPAsm_AsmException_SetLine(V2MPAsm_AsmException* ex, size_t line);
void V2MPAsm_AsmException_SetColumn(V2MPAsm_AsmException* ex, size_t column);

void V2MPAsm_AsmException_SetWarning(V2MPAsm_AsmException* ex, V2MPAsm_AsmWarningType warning);
void V2MPAsm_AsmException_SetError(V2MPAsm_AsmException* ex, V2MPAsm_AsmErrorType error);

#endif // V2MPASM_ASMEXCEPTION_INTERNAL_H
