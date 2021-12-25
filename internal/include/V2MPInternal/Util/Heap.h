#ifndef V2MPINTERNAL_HEAP_H
#define V2MPINTERNAL_HEAP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPI_HeapFunctions
{
	void* (*mallocFunc)(size_t);
	void* (*reallocFunc)(void*, size_t);
	void* (*callocFunc)(size_t, size_t);
	void  (*freeFunc)(void*);
} V2MPI_HeapFunctions;

void V2MPI_Heap_SetHeapFunctions(V2MPI_HeapFunctions functions);
void V2MPI_Heap_ResetHeapFunctions(void);

void* V2MPI_Heap_Malloc(size_t size);
void* V2MPI_Heap_Realloc(void* ptr, size_t newSize);
void* V2MPI_Heap_Calloc(size_t numElements, size_t elementSize);
void V2MPI_Heap_Free(void* ptr);

#define V2MPI_MALLOC(size) V2MPI_Heap_Malloc(size)
#define V2MPI_REALLOC(ptr, newSize) V2MPI_Heap_Realloc(ptr, newSize)
#define V2MPI_CALLOC(numElements, elementSize) V2MPI_Heap_Calloc(numElements, elementSize)
#define V2MPI_FREE(ptr) V2MPI_Heap_Free(ptr)

#define V2MPI_CALLOC_STRUCT(structType) ((structType*)V2MPI_CALLOC(1, sizeof(structType)))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPINTERNAL_HEAP_H
