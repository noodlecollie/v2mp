#ifndef V2MPASM_PARSEEXCEPTION_H
#define V2MPASM_PARSEEXCEPTION_H

#include <stddef.h>
#include "V2MPAsm/LibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPAsm_ParseException V2MPAsm_ParseException;

typedef enum V2MPAsm_ParseException_Type
{
	V2MPASM_PARSEEXCEPTION_WARNING = 0,
	V2MPASM_PARSEEXCEPTION_ERROR
} V2MPAsm_ParseException_Type;

API_V2MPASM V2MPAsm_ParseException_Type V2MPAsm_ParseException_GetType(const V2MPAsm_ParseException* exception);
API_V2MPASM void V2MPAsm_ParseException_ToString(const V2MPAsm_ParseException* exception, char* buffer, size_t length);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_PARSEEXCEPTION_H
