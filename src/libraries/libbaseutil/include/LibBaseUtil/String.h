#ifndef BASEUTIL_STRING_H
#define BASEUTIL_STRING_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

char* BaseUtil_String_Duplicate(const char* origString);

// Returns a pointer to where the new terminator of the string should be
// if trailing whitespace were to be removed.
const char* BaseUtil_String_EndWithoutWhitespace(const char* origString);

// Returns a pointer to the first non-whitespace character of the string.
const char* BaseUtil_String_BeginWithoutWhitespace(const char* origString);

// Returns the first character of whitespace in the string.
const char* BaseUtil_String_NextWhitespace(const char* string);

bool BaseUtil_String_CharIsAlphanumericOrUnderscore(char ch);

bool BaseUtil_String_ToLongInt(const char* str, const char** end, int base, long int* output);

// Returns 10 for decimal, 16 for hex, 2 for binary, or 0 if
// the base could not be determined.
// Note that this function does not validate whether the entire
// string represents a number - it only looks at the prefix.
int BaseUtil_String_GetBaseFromNumberPrefix(const char* str);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BASEUTIL_STRING_H
