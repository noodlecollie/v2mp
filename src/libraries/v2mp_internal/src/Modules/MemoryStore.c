#include "V2MPInternal/Modules/MemoryStore.h"
#include "BaseUtil/Heap.h"
#include "V2MPInternal/Defs.h"

struct V2MP_MemoryStore
{
	V2MP_Byte* totalMemory;
	size_t totalMemorySizeInBytes;
};

static void FreeAllMemory(V2MP_MemoryStore* mem)
{
	if ( !mem )
	{
		return;
	}

	if ( mem->totalMemory )
	{
		BASEUTIL_FREE(mem->totalMemory);
	}

	mem->totalMemory = NULL;
	mem->totalMemorySizeInBytes = 0;
}

V2MP_MemoryStore* V2MP_MemoryStore_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MP_MemoryStore);
}

void V2MP_MemoryStore_DeinitAndFree(V2MP_MemoryStore* mem)
{
	if ( !mem )
	{
		return;
	}

	FreeAllMemory(mem);
	BASEUTIL_FREE(mem);
}

bool V2MP_MemoryStore_AllocateTotalMemory(V2MP_MemoryStore* mem, size_t sizeInBytes)
{
	if ( !mem || (sizeInBytes & 0x1) )
	{
		return false;
	}

	FreeAllMemory(mem);

	mem->totalMemory = BASEUTIL_MALLOC(sizeInBytes);

	if ( !mem->totalMemory )
	{
		return false;
	}

	mem->totalMemorySizeInBytes = sizeInBytes;
	return true;
}

size_t V2MP_MemoryStore_GetTotalMemorySize(const V2MP_MemoryStore* mem)
{
	return mem ? mem->totalMemorySizeInBytes : 0;
}

V2MP_Byte* V2MP_MemoryStore_GetPtrToBase(V2MP_MemoryStore* mem)
{
	return (V2MP_Byte*)V2MP_MemoryStore_GetConstPtrToBase(mem);
}

const V2MP_Byte* V2MP_MemoryStore_GetConstPtrToBase(const V2MP_MemoryStore* mem)
{
	return mem ? mem->totalMemory : NULL;
}

bool V2MP_MemoryStore_LoadWord(
	const V2MP_MemoryStore* mem,
	size_t address,
	V2MP_Word* outWord
)
{
	const V2MP_Byte* rawData;

	if ( !mem || !outWord )
	{
		return false;
	}

	rawData = V2MP_MemoryStore_GetConstPtrToRange(mem, address, sizeof(V2MP_Word));

	if ( !rawData )
	{
		return false;
	}

	*outWord = *((const V2MP_Word*)rawData);
	return true;
}

bool V2MP_MemoryStore_StoreWord(
	V2MP_MemoryStore* mem,
	size_t address,
	V2MP_Word inWord
)
{
	V2MP_Byte* rawData;

	if ( !mem )
	{
		return false;
	}

	rawData = V2MP_MemoryStore_GetPtrToRange(mem, address, sizeof(V2MP_Word));

	if ( !rawData )
	{
		return false;
	}

	*((V2MP_Word*)rawData) = inWord;
	return true;
}

V2MP_Byte* V2MP_MemoryStore_GetPtrToRange(
	V2MP_MemoryStore* mem,
	size_t base,
	size_t length
)
{
	return (V2MP_Byte*)V2MP_MemoryStore_GetConstPtrToRange(mem, base, length);
}

const V2MP_Byte* V2MP_MemoryStore_GetConstPtrToRange(
	const V2MP_MemoryStore* mem,
	size_t base,
	size_t length
)
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
