#ifndef V2MPINTERNAL_UTIL_H
#define V2MPINTERNAL_UTIL_H

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define V2MPI_ZERO_STRUCT_PTR(ptr) memset((ptr), 0, sizeof(*(ptr)))

#define V2MPI_MIN(a, b) ((a) < (b) ? (a) : (b))
#define V2MPI_MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPINTERNAL_UTIL_H
