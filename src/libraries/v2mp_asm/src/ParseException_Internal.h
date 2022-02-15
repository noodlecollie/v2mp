#ifndef V2MPASM_PARSEEXCEPTION_INTERNAL_H
#define V2MPASM_PARSEEXCEPTION_INTERNAL_H

#include "V2MPAsm/ParseException.h"

struct V2MPAsm_ParseContext;

#define V2MPASM_PARSEWARNINGTYPE_LIST \
	LIST_ITEM(PARSEWARNING_UNSPECIFIED = 0, "Unspecified warning.")

#define V2MPASM_PARSEERRORTYPE_LIST \
	LIST_ITEM(PARSEERROR_UNSPECIFIED = 0, "Unspecified error.") \
	LIST_ITEM(PARSEERROR_INTERNAL, "Unexpected internal error.") \
	LIST_ITEM(PARSEERROR_UNIMPLEMENTED, "Behaviour unimplemented.") \
	LIST_ITEM(PARSEERROR_UNRECOGNISED_INSTRUCTION, "Unrecognised instruction.")

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
	struct V2MPAsm_ParseContext* context;
	size_t line;
	size_t column;
	char* customDescription;
};

const char* V2MPAsm_ParseException_GetWarningString(V2MPAsm_ParseWarningType warning);
const char* V2MPAsm_ParseException_GetErrorString(V2MPAsm_ParseErrorType error);

V2MPAsm_ParseException* V2MPAsm_ParseException_AllocateAndInit(void);
void V2MPAsm_ParseException_DeinitAndFree(V2MPAsm_ParseException* exception);

struct V2MPAsm_ParseContext* V2MPAsm_ParseException_GetContext(const V2MPAsm_ParseException* exception);
void V2MPAsm_ParseException_SetContext(V2MPAsm_ParseException* exception, struct V2MPAsm_ParseContext* context);

void V2MPAsm_ParseException_SetWarning(V2MPAsm_ParseException* exception, V2MPAsm_ParseWarningType type);
void V2MPAsm_ParseException_SetError(V2MPAsm_ParseException* exception, V2MPAsm_ParseErrorType type);
const char* V2MPAsm_ParseException_GetWarningOrErrorString(const V2MPAsm_ParseException* exception);

const char* V2MPAsm_ParseException_GetCustomDescription(const V2MPAsm_ParseException* exception);
void V2MPAsm_ParseException_SetCustomDescription(V2MPAsm_ParseException* exception, const char* description);

size_t V2MPAsm_ParseException_GetLine(const V2MPAsm_ParseException* exception);
size_t V2MPAsm_ParseException_GetColumn(const V2MPAsm_ParseException* exception);
void V2MPAsm_ParseException_SetLineAndColumn(V2MPAsm_ParseException* exception, size_t line, size_t column);

#endif // V2MPASM_PARSEEXCEPTION_INTERNAL_H
