#ifndef BASEUTIL_UTIL_H
#define BASEUTIL_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void* BaseUtil_MemSet(void* ptr, int value, size_t numBytes);

// Wraps exit() so that uthash "unreachable code" warnings are not thrown on Windows.
void BaseUtil_ExitProcess(int code);

static inline void* BaseUtil_SetToZero(void* ptr, size_t numBytes)
{
	return BaseUtil_MemSet(ptr, 0, numBytes);
}

#define BASEUTIL_ZERO_STRUCT_PTR(ptr) BaseUtil_SetToZero((ptr), sizeof(*(ptr)))

#define BASEUTIL_MIN(a, b) ((a) < (b) ? (a) : (b))
#define BASEUTIL_MAX(a, b) ((a) > (b) ? (a) : (b))

#define BASEUTIL_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BASEUTIL_UTIL_H
