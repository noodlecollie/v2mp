#ifndef V2MPASM_ASMEXCEPTION_H
#define V2MPASM_ASMEXCEPTION_H

#include <stddef.h>
#include <stdbool.h>
#include "V2MPAsm/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPAsm_AsmException V2MPAsm_AsmException;

typedef enum V2MPAsm_AsmExceptionType
{
	V2MPASM_EXCEPTION_NONE = 0,
	V2MPASM_EXCEPTION_WARNING,
	V2MPASM_EXCEPTION_ERROR,
} V2MPAsm_AsmExceptionType;

size_t V2MPAsm_AsmException_GetLine(const V2MPAsm_AsmException* ex);
size_t V2MPAsm_AsmException_GetColumn(const V2MPAsm_AsmException* ex);
V2MPAsm_AsmExceptionType V2MPAsm_AsmException_GetExceptionType(const V2MPAsm_AsmException* ex);

// True if the exception holds a warning or error, or false otherwise.
bool V2MPAsm_AsmException_IsValid(const V2MPAsm_AsmException* ex);

// Only valid if the exception type is V2MPASM_EXCEPTION_WARNING
V2MPAsm_AsmWarningType V2MPAsm_AsmException_GetWarningType(const V2MPAsm_AsmException* ex);

// Only valid if the exception type is V2MPASM_EXCEPTION_ERROR
V2MPAsm_AsmErrorType V2MPAsm_AsmException_GetErrorType(const V2MPAsm_AsmException* ex);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_ASMEXCEPTION_H
