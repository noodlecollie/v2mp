#ifndef V2MPASM_PARSEEXCEPTION_INTERNAL_H
#define V2MPASM_PARSEEXCEPTION_INTERNAL_H

#include <stdarg.h>
#include "V2MPAsm/ParseException.h"

struct V2MPAsm_ParseContext;

#define V2MPASM_PARSEEXCEPTION_FORMAT_DESC_MAX_LENGTH 512

#define V2MPASM_PARSEWARNINGTYPE_LIST \
	LIST_ITEM(PARSEWARNING_UNSPECIFIED = 0, "unspecified", "Unspecified warning.") \
	LIST_ITEM(PARSEWARNING_ARG_OUT_OF_RANGE, "arg-out-of-range", "Instruction argument is out of range.")

#define V2MPASM_PARSEERRORTYPE_LIST \
	LIST_ITEM(PARSEERROR_UNSPECIFIED = 0, "unspecified", "Unspecified error.") \
	LIST_ITEM(PARSEERROR_INTERNAL, "internal-error", "Unexpected internal error.") \
	LIST_ITEM(PARSEERROR_UNIMPLEMENTED, "unimplemented", "Behaviour unimplemented.") \
	LIST_ITEM(PARSEERROR_UNRECOGNISED_INSTRUCTION, "unrecognised-instruction", "Unrecognised instruction.") \
	LIST_ITEM(PARSEERROR_UNEXPECTED_TOKEN, "unexpected-token", "Unexpected token encountered.") \
	LIST_ITEM(PARSEERROR_UNRECOGNISED_TOKEN, "unrecognised-token", "Unrecognised token.") \
	LIST_ITEM(PARSEERROR_UNTERMINATED_TOKEN, "unterminated-token", "Unterminated token.") \
	LIST_ITEM(PARSEERROR_INCORRECT_ARGS, "incorrect-args", "Incorrect arguments for instruction.") \
	LIST_ITEM(PARSEERROR_INVALID_NUMERIC_LITERAL, "invalid-numeric-literal", "Invalid numeric literal.")

#define LIST_ITEM(value, id, desc) value,
typedef enum V2MPAsm_ParseWarningType
{
	V2MPASM_PARSEWARNINGTYPE_LIST
} V2MPAsm_ParseWarningType;
#undef LIST_ITEM

#define LIST_ITEM(value, id, desc) value,
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
const char* V2MPAsm_ParseException_GetWarningID(V2MPAsm_ParseWarningType warning);
const char* V2MPAsm_ParseException_GetErrorString(V2MPAsm_ParseErrorType error);
const char* V2MPAsm_ParseException_GetErrorID(V2MPAsm_ParseErrorType error);

V2MPAsm_ParseException* V2MPAsm_ParseException_AllocateAndInit(void);
void V2MPAsm_ParseException_DeinitAndFree(V2MPAsm_ParseException* exception);

struct V2MPAsm_ParseContext* V2MPAsm_ParseException_GetContext(const V2MPAsm_ParseException* exception);
void V2MPAsm_ParseException_SetContext(V2MPAsm_ParseException* exception, struct V2MPAsm_ParseContext* context);

void V2MPAsm_ParseException_SetWarning(V2MPAsm_ParseException* exception, V2MPAsm_ParseWarningType type);
void V2MPAsm_ParseException_SetError(V2MPAsm_ParseException* exception, V2MPAsm_ParseErrorType type);
const char* V2MPAsm_ParseException_GetWarningOrErrorString(const V2MPAsm_ParseException* exception);
const char* V2MPAsm_ParseException_GetWarningOrErrorID(const V2MPAsm_ParseException* exception);

const char* V2MPAsm_ParseException_GetCustomDescription(const V2MPAsm_ParseException* exception);
void V2MPAsm_ParseException_SetCustomDescription(V2MPAsm_ParseException* exception, const char* description);
void V2MPAsm_ParseException_FormatCustomDescription(V2MPAsm_ParseException* exception, const char* format, ...);
void V2MPAsm_ParseException_FormatCustomDescriptionV(V2MPAsm_ParseException* exception, const char* format, va_list args);

size_t V2MPAsm_ParseException_GetLine(const V2MPAsm_ParseException* exception);
size_t V2MPAsm_ParseException_GetColumn(const V2MPAsm_ParseException* exception);
void V2MPAsm_ParseException_SetLineAndColumn(V2MPAsm_ParseException* exception, size_t line, size_t column);

#endif // V2MPASM_PARSEEXCEPTION_INTERNAL_H
