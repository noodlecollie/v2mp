#ifndef BASEUTIL_UTIL_H
#define BASEUTIL_UTIL_H

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BASEUTIL_ZERO_STRUCT_PTR(ptr) memset((ptr), 0, sizeof(*(ptr)))

#define BASEUTIL_MIN(a, b) ((a) < (b) ? (a) : (b))
#define BASEUTIL_MAX(a, b) ((a) > (b) ? (a) : (b))

#define BASEUTIL_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BASEUTIL_UTIL_H
