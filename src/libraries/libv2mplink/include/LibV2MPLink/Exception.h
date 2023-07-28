#ifndef V2MPLINL_EXCEPTION_H
#define V2MPLINL_EXCEPTION_H

#include <stddef.h>
#include "LibV2MPLink/LibExport.gen.h"

typedef enum V2MPLink_Exception_Type
{
	V2MPLINK_EXCEPTION_WARNING = 0,
	V2MPLINK_EXCEPTION_ERROR
} V2MPLink_Exception_Type;

struct V2MPLink_Exception;

LIBV2MPLINK_PUBLIC(V2MPLink_Exception_Type) V2MPLink_Exception_GetType(const struct V2MPLink_Exception* exception);
LIBV2MPLINK_PUBLIC(const char*) V2MPLink_Exception_GetID(const struct V2MPLink_Exception* exception);
LIBV2MPLINK_PUBLIC(size_t) V2MPLink_Exception_ToString(const struct V2MPLink_Exception* exception, char* buffer, size_t length);

LIBV2MPLINK_PUBLIC(size_t) V2MPLink_ExceptionList_GetWarningsListSize(void);
LIBV2MPLINK_PUBLIC(const char*) V2MPLink_ExceptionList_GetWarningStringID(size_t index);
LIBV2MPLINK_PUBLIC(const char*) V2MPLink_ExceptionList_GetWarningDescription(size_t index);

LIBV2MPLINK_PUBLIC(size_t) V2MPLink_ExceptionList_GetErrorsListSize(void);
LIBV2MPLINK_PUBLIC(const char*) V2MPLink_ExceptionList_GetErrorStringID(size_t index);
LIBV2MPLINK_PUBLIC(const char*) V2MPLink_ExceptionList_GetErrorDescription(size_t index);

#endif // V2MPLINL_EXCEPTION_H
