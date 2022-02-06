#include <stdlib.h>
#include "V2MPInternal/Util/Heap.h"

// According to Windows, functions like malloc() are dllimported, so they don't
// necessarily have a static address. Instead, we have to wrap them.

static void* LocalMalloc(size_t size)
{
	return malloc(size);
}

static void* LocalRealloc(void* ptr, size_t newSize)
{
	return realloc(ptr, newSize);
}

static void* LocalCalloc(size_t numItems, size_t itemSize)
{
	return calloc(numItems, itemSize);
}

static void LocalFree(void* ptr)
{
	free(ptr);
}

V2MP_HeapFunctions LocalHeapFunctions =
{
	&LocalMalloc,
	&LocalRealloc,
	&LocalCalloc,
	&LocalFree
};

void V2MP_Heap_SetHeapFunctions(V2MP_HeapFunctions functions)
{
	if ( !functions.mallocFunc )
	{
		functions.mallocFunc = &LocalMalloc;
	}

	if ( !functions.reallocFunc )
	{
		functions.reallocFunc = &LocalRealloc;
	}

	if ( !functions.callocFunc )
	{
		functions.callocFunc = &LocalCalloc;
	}

	if ( !functions.freeFunc )
	{
		functions.freeFunc = &LocalFree;
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
