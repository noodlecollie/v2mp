#ifndef V2MPASM_PARSEEXCEPTION_H
#define V2MPASM_PARSEEXCEPTION_H

#include <stddef.h>
#include "LibV2MPAsm/LibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum V2MPAsm_Exception_Type
{
	V2MPASM_EXCEPTION_WARNING = 0,
	V2MPASM_EXCEPTION_ERROR
} V2MPAsm_Exception_Type;

struct V2MPAsm_Exception;

API_LIBV2MPASM V2MPAsm_Exception_Type V2MPAsm_Exception_GetType(const struct V2MPAsm_Exception* exception);
API_LIBV2MPASM const char* V2MPAsm_Exception_GetID(const struct V2MPAsm_Exception* exception);
API_LIBV2MPASM size_t V2MPAsm_Exception_ToString(const struct V2MPAsm_Exception* exception, char* buffer, size_t length);

API_LIBV2MPASM size_t V2MPAsm_ExceptionList_GetWarningsListSize(void);
API_LIBV2MPASM const char* V2MPAsm_ExceptionList_GetWarningStringID(size_t index);
API_LIBV2MPASM const char* V2MPAsm_ExceptionList_GetWarningDescription(size_t index);

API_LIBV2MPASM size_t V2MPAsm_ExceptionList_GetErrorsListSize(void);
API_LIBV2MPASM const char* V2MPAsm_ExceptionList_GetErrorStringID(size_t index);
API_LIBV2MPASM const char* V2MPAsm_ExceptionList_GetErrorDescription(size_t index);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_PARSEEXCEPTION_H
