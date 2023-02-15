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

API_V2MPASM V2MPAsm_Exception_Type V2MPAsm_Exception_GetType(const struct V2MPAsm_Exception* exception);
API_V2MPASM const char* V2MPAsm_Exception_GetID(const struct V2MPAsm_Exception* exception);
API_V2MPASM size_t V2MPAsm_Exception_ToString(const struct V2MPAsm_Exception* exception, char* buffer, size_t length);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_PARSEEXCEPTION_H
