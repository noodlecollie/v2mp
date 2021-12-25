#include <stdlib.h>
#include "V2MPInternal/Util/Heap.h"

V2MPI_HeapFunctions LocalHeapFunctions =
{
	&malloc,
	&realloc,
	&calloc,
	&free
};

void V2MPI_Heap_SetHeapFunctions(V2MPI_HeapFunctions functions)
{
	if ( !functions.mallocFunc )
	{
		functions.mallocFunc = &malloc;
	}

	if ( !functions.reallocFunc )
	{
		functions.reallocFunc = &realloc;
	}

	if ( !functions.callocFunc )
	{
		functions.callocFunc = &calloc;
	}

	if ( !functions.freeFunc )
	{
		functions.freeFunc = &free;
	}

	LocalHeapFunctions = functions;
}

void V2MPI_Heap_ResetHeapFunctions(void)
{
	V2MPI_HeapFunctions functions = { NULL, NULL, NULL, NULL };
	V2MPI_Heap_SetHeapFunctions(functions);
}

void* V2MPI_Heap_Malloc(size_t size)
{
	return LocalHeapFunctions.mallocFunc(size);
}

void* V2MPI_Heap_Realloc(void* ptr, size_t newSize)
{
	return LocalHeapFunctions.reallocFunc(ptr, newSize);
}

void* V2MPI_Heap_Calloc(size_t numElements, size_t elementSize)
{
	return LocalHeapFunctions.callocFunc(numElements, elementSize);
}

void V2MPI_Heap_Free(void* ptr)
{
	LocalHeapFunctions.freeFunc(ptr);
}
