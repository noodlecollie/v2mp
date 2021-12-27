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
