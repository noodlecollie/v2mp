#include <stdlib.h>
#include "BaseUtil/Heap.h"

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

BaseUtil_HeapFunctions LocalHeapFunctions =
{
	&LocalMalloc,
	&LocalRealloc,
	&LocalCalloc,
	&LocalFree
};

void BaseUtil_Heap_SetHeapFunctions(BaseUtil_HeapFunctions functions)
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

void BaseUtil_Heap_ResetHeapFunctions(void)
{
	BaseUtil_HeapFunctions functions = { NULL, NULL, NULL, NULL };
	BaseUtil_Heap_SetHeapFunctions(functions);
}

void* BaseUtil_Heap_Malloc(size_t size)
{
	return LocalHeapFunctions.mallocFunc(size);
}

void* BaseUtil_Heap_Realloc(void* ptr, size_t newSize)
{
	return LocalHeapFunctions.reallocFunc(ptr, newSize);
}

void* BaseUtil_Heap_Calloc(size_t numElements, size_t elementSize)
{
	return LocalHeapFunctions.callocFunc(numElements, elementSize);
}

void BaseUtil_Heap_Free(void* ptr)
{
	LocalHeapFunctions.freeFunc(ptr);
}
