#ifndef V2MP_UTIL_HEAP_H
#define V2MP_UTIL_HEAP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_HeapFunctions
{
	void* (*mallocFunc)(size_t);
	void* (*reallocFunc)(void*, size_t);
	void* (*callocFunc)(size_t, size_t);
	void  (*freeFunc)(void*);
} V2MP_HeapFunctions;

void V2MP_Heap_SetHeapFunctions(V2MP_HeapFunctions functions);
void V2MP_Heap_ResetHeapFunctions(void);

void* V2MP_Heap_Malloc(size_t size);
void* V2MP_Heap_Realloc(void* ptr, size_t newSize);
void* V2MP_Heap_Calloc(size_t numElements, size_t elementSize);
void V2MP_Heap_Free(void* ptr);

#define V2MP_MALLOC(size) V2MP_Heap_Malloc(size)
#define V2MP_REALLOC(ptr, newSize) V2MP_Heap_Realloc(ptr, newSize)
#define V2MP_CALLOC(numElements, elementSize) V2MP_Heap_Calloc(numElements, elementSize)
#define V2MP_FREE(ptr) V2MP_Heap_Free(ptr)

#define V2MP_CALLOC_STRUCT(structType) ((structType*)V2MP_CALLOC(1, sizeof(structType)))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_UTIL_HEAP_H
