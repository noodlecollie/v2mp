#include <stdlib.h>
#include "V2MPInternal/Util/Heap.h"

V2MP_HeapFunctions LocalHeapFunctions =
{
	&malloc,
	&realloc,
	&calloc,
	&free
};

void V2MP_Heap_SetHeapFunctions(V2MP_HeapFunctions functions)
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

void V2MP_Heap_ResetHeapFunctions(void)
{
	V2MP_HeapFunctions functions = { NULL, NULL, NULL, NULL };
	V2MP_Heap_SetHeapFunctions(functions);
}

void* V2MP_Heap_Malloc(size_t size)
{
	return LocalHeapFunctions.mallocFunc(size);
}

void* V2MP_Heap_Realloc(void* ptr, size_t newSize)
{
	return LocalHeapFunctions.reallocFunc(ptr, newSize);
}

void* V2MP_Heap_Calloc(size_t numElements, size_t elementSize)
{
	return LocalHeapFunctions.callocFunc(numElements, elementSize);
}

void V2MP_Heap_Free(void* ptr)
{
	LocalHeapFunctions.freeFunc(ptr);
}
