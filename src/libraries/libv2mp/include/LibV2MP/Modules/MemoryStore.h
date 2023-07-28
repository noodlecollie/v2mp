#ifndef V2MPINTERNAL_MODULES_MEMORYSTORE_H
#define V2MPINTERNAL_MODULES_MEMORYSTORE_H

#include <stddef.h>
#include <stdbool.h>
#include "LibV2MP/LibExport.gen.h"
#include "LibV2MP/Defs.h"

typedef struct V2MP_MemoryStore V2MP_MemoryStore;

V2MP_MemoryStore* V2MP_MemoryStore_AllocateAndInit(void);
void V2MP_MemoryStore_DeinitAndFree(V2MP_MemoryStore* mem);

// Total memory must be a multiple of sizeof(V2MP_Word), otherwise allocation will fail.
bool V2MP_MemoryStore_AllocateTotalMemory(V2MP_MemoryStore* mem, size_t sizeInBytes);

size_t V2MP_MemoryStore_GetTotalMemorySize(const V2MP_MemoryStore* mem);

V2MP_Byte* V2MP_MemoryStore_GetPtrToBase(V2MP_MemoryStore* mem);
const V2MP_Byte* V2MP_MemoryStore_GetConstPtrToBase(const V2MP_MemoryStore* mem);

bool V2MP_MemoryStore_LoadWord(
	const V2MP_MemoryStore* mem,
	size_t address,
	V2MP_Word* outWord
);

bool V2MP_MemoryStore_StoreWord(
	V2MP_MemoryStore* mem,
	size_t address,
	V2MP_Word inWord
);

// Returns null if the range would exceed the total memory area.
V2MP_Byte* V2MP_MemoryStore_GetPtrToRange(
	V2MP_MemoryStore* mem,
	size_t base,
	size_t length
);

// Returns null if the range would exceed the total memory area.
const V2MP_Byte* V2MP_MemoryStore_GetConstPtrToRange(
	const V2MP_MemoryStore* mem,
	size_t base,
	size_t length
);

#endif // V2MPINTERNAL_MODULES_MEMORYSTORE_H
