#ifndef V2MP_UTIL_UTIL_H
#define V2MP_UTIL_UTIL_H

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define V2MP_ZERO_STRUCT_PTR(ptr) memset((ptr), 0, sizeof(*(ptr)))

#define V2MP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define V2MP_MAX(a, b) ((a) > (b) ? (a) : (b))

#define V2MP_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_UTIL_UTIL_H
