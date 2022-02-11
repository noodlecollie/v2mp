#ifndef BASEUTIL_STRING_H
#define BASEUTIL_STRING_H

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BASEUTIL_STRING_H
