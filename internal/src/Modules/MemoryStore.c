#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Util/Heap.h"
#include "V2MPInternal/Defs.h"

struct V2MP_MemoryStoreRenameMe
{
	V2MP_Byte* totalMemory;
	size_t totalMemorySizeInBytes;
};

static void FreeAllMemory(V2MP_MemoryStoreRenameMe* mem)
{
	if ( !mem )
	{
		return;
	}

	if ( mem->totalMemory )
	{
		V2MP_FREE(mem->totalMemory);
	}

	mem->totalMemory = NULL;
	mem->totalMemorySizeInBytes = 0;
}

V2MP_MemoryStoreRenameMe* V2MP_MemoryStoreRenameMe_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_MemoryStoreRenameMe);
}

void V2MP_MemoryStoreRenameMe_DeinitAndFree(V2MP_MemoryStoreRenameMe* mem)
{
	if ( !mem )
	{
		return;
	}

	FreeAllMemory(mem);
	V2MP_FREE(mem);
}

bool V2MP_MemoryStoreRenameMe_AllocateTotalMemory(V2MP_MemoryStoreRenameMe* mem, size_t sizeInBytes)
{
	if ( !mem || (sizeInBytes & 0x1) )
	{
		return false;
	}

	FreeAllMemory(mem);

	mem->totalMemory = V2MP_MALLOC(sizeInBytes);

	if ( !mem->totalMemory )
	{
		return false;
	}

	mem->totalMemorySizeInBytes = sizeInBytes;
	return true;
}

size_t V2MP_MemoryStoreRenameMe_GetTotalMemorySize(const V2MP_MemoryStoreRenameMe* mem)
{
	return mem ? mem->totalMemorySizeInBytes : 0;
}

V2MP_Byte* V2MP_MemoryStoreRenameMe_GetPtrToRange(V2MP_MemoryStoreRenameMe* mem, size_t base, size_t length)
{
	if ( !mem || !mem->totalMemory || mem->totalMemorySizeInBytes < 1 )
	{
		return NULL;
	}

	// The following checks are carefully constructed to avoid calculating
	// values that are outside of the range of a size_t:

	if ( base >= mem->totalMemorySizeInBytes )
	{
		// Begins out of range.
		return NULL;
	}

	if ( mem->totalMemorySizeInBytes - base < length )
	{
		// Ends out of range.
		return NULL;
	}

	return &mem->totalMemory[base];
}
