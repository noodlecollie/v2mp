#ifndef V2MPASM_PARSEEXCEPTION_INTERNAL_H
#define V2MPASM_PARSEEXCEPTION_INTERNAL_H

#include "V2MPAsm/ParseException.h"

#define V2MPASM_PARSEWARNINGTYPE_LIST \
	LIST_ITEM(PARSEWARNING_UNSPECIFIED = 0, "Unspecified warning")

#define V2MPASM_PARSEERRORTYPE_LIST \
	LIST_ITEM(PARSEERROR_UNSPECIFIED = 0, "Unspecified error")

#define LIST_ITEM(value, desc) value,
typedef enum V2MPAsm_ParseWarningType
{
	V2MPASM_PARSEWARNINGTYPE_LIST
} V2MPAsm_ParseWarningType;
#undef LIST_ITEM

#define LIST_ITEM(value, desc) value,
typedef enum V2MPAsm_ParseErrorType
{
	V2MPASM_PARSEERRORTYPE_LIST
} V2MPAsm_ParseErrorType;
#undef LIST_ITEM

struct V2MPAsm_ParseException
{
	V2MPAsm_ParseException_Type type;
	int warningOrErrorType;
};

V2MPAsm_ParseException* V2MPAsm_ParseException_AllocateAndInit(void);
void V2MPAsm_ParseException_DeallocateAndFree(V2MPAsm_ParseException* exception);

void V2MPAsm_ParseException_SetWarning(V2MPAsm_ParseException* exception, V2MPAsm_ParseWarningType type);
void V2MPAsm_ParseException_SetError(V2MPAsm_ParseException* exception, V2MPAsm_ParseErrorType type);

#endif // V2MPASM_PARSEEXCEPTION_INTERNAL_H
