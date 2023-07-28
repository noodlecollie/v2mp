#ifndef V2MPASM_EXCEPTION_H
#define V2MPASM_EXCEPTION_H

#include <stddef.h>
#include "LibV2MPAsm/LibExport.gen.h"

typedef enum V2MPAsm_Exception_Type
{
	V2MPASM_EXCEPTION_WARNING = 0,
	V2MPASM_EXCEPTION_ERROR
} V2MPAsm_Exception_Type;

struct V2MPAsm_Exception;

LIBV2MPASM_PUBLIC(V2MPAsm_Exception_Type) V2MPAsm_Exception_GetType(const struct V2MPAsm_Exception* exception);
LIBV2MPASM_PUBLIC(const char*) V2MPAsm_Exception_GetID(const struct V2MPAsm_Exception* exception);
LIBV2MPASM_PUBLIC(size_t) V2MPAsm_Exception_ToString(const struct V2MPAsm_Exception* exception, char* buffer, size_t length);

LIBV2MPASM_PUBLIC(size_t) V2MPAsm_ExceptionList_GetWarningsListSize(void);
LIBV2MPASM_PUBLIC(const char*) V2MPAsm_ExceptionList_GetWarningStringID(size_t index);
LIBV2MPASM_PUBLIC(const char*) V2MPAsm_ExceptionList_GetWarningDescription(size_t index);

LIBV2MPASM_PUBLIC(size_t) V2MPAsm_ExceptionList_GetErrorsListSize(void);
LIBV2MPASM_PUBLIC(const char*) V2MPAsm_ExceptionList_GetErrorStringID(size_t index);
LIBV2MPASM_PUBLIC(const char*) V2MPAsm_ExceptionList_GetErrorDescription(size_t index);

#endif // V2MPASM_EXCEPTION_H
