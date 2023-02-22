#ifndef V2MPLINL_EXCEPTION_H
#define V2MPLINL_EXCEPTION_H

#include <stddef.h>
#include "LibV2MPLink/LibExport.gen.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum V2MPLink_Exception_Type
{
	V2MPLINK_EXCEPTION_WARNING = 0,
	V2MPLINK_EXCEPTION_ERROR
} V2MPLink_Exception_Type;

struct V2MPLink_Exception;

API_LIBV2MPLINK V2MPLink_Exception_Type V2MPLink_Exception_GetType(const struct V2MPLink_Exception* exception);
API_LIBV2MPLINK const char* V2MPLink_Exception_GetID(const struct V2MPLink_Exception* exception);
API_LIBV2MPLINK size_t V2MPLink_Exception_ToString(const struct V2MPLink_Exception* exception, char* buffer, size_t length);

API_LIBV2MPLINK size_t V2MPLink_ExceptionList_GetWarningsListSize(void);
API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetWarningStringID(size_t index);
API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetWarningDescription(size_t index);

API_LIBV2MPLINK size_t V2MPLink_ExceptionList_GetErrorsListSize(void);
API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetErrorStringID(size_t index);
API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetErrorDescription(size_t index);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPLINL_EXCEPTION_H
