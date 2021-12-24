#ifndef V2MP_UTIL_H
#define V2MP_UTIL_H

#include <string.h>
#include <stdlib.h>

#define V2MP_ZERO_STRUCT_PTR(ptr) memset((ptr), 0, sizeof(*(ptr)))
#define V2MP_CALLOC_STRUCT(structType) ((structType*)calloc(1, sizeof(structType)))

#define V2MP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define V2MP_MAX(a, b) ((a) > (b) ? (a) : (b))

#endif // V2MP_UTIL_H
