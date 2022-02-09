#ifndef BASEUTIL_HEAP_H
#define BASEUTIL_HEAP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BaseUtil_HeapFunctions
{
	void* (*mallocFunc)(size_t);
	void* (*reallocFunc)(void*, size_t);
	void* (*callocFunc)(size_t, size_t);
	void  (*freeFunc)(void*);
} BaseUtil_HeapFunctions;

void BaseUtil_Heap_SetHeapFunctions(BaseUtil_HeapFunctions functions);
void BaseUtil_Heap_ResetHeapFunctions(void);

void* BaseUtil_Heap_Malloc(size_t size);
void* BaseUtil_Heap_Realloc(void* ptr, size_t newSize);
void* BaseUtil_Heap_Calloc(size_t numElements, size_t elementSize);
void BaseUtil_Heap_Free(void* ptr);

#define BASEUTIL_MALLOC(size) BaseUtil_Heap_Malloc(size)
#define BASEUTIL_REALLOC(ptr, newSize) BaseUtil_Heap_Realloc(ptr, newSize)
#define BASEUTIL_CALLOC(numElements, elementSize) BaseUtil_Heap_Calloc(numElements, elementSize)
#define BASEUTIL_FREE(ptr) BaseUtil_Heap_Free(ptr)

#define BASEUTIL_MALLOC_STRUCT(structType) ((structType*)BASEUTIL_MALLOC(sizeof(structType)))
#define BASEUTIL_CALLOC_STRUCT(structType) ((structType*)BASEUTIL_CALLOC(1, sizeof(structType)))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BASEUTIL_HEAP_H
