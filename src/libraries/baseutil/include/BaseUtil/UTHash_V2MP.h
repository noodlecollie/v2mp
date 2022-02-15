#ifndef BASEUTIL_UTHASH_V2MP_H
#define BASEUTIL_UTHASH_V2MP_H

#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"

#define uthash_malloc(sz) BASEUTIL_MALLOC(sz)
#define uthash_free(ptr, sz) BASEUTIL_FREE(ptr)
#define uthash_bzero(a,n) BaseUtil_SetToZero(a, n)

#include "uthash/uthash.h"

#endif // BASEUTIL_UTHASH_V2MP_H
