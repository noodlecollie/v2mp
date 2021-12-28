#ifndef V2MP_MODULES_MEMORYSTORE_H
#define V2MP_MODULES_MEMORYSTORE_H

#include <stddef.h>
#include <stdbool.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_MemoryStoreRenameMe V2MP_MemoryStoreRenameMe;

V2MP_MemoryStoreRenameMe* V2MP_MemoryStoreRenameMe_AllocateAndInit(void);
void V2MP_MemoryStoreRenameMe_DeinitAndFree(V2MP_MemoryStoreRenameMe* mem);

// Total memory must be a multiple of sizeof(V2MP_Word), otherwise allocation will fail.
bool V2MP_MemoryStoreRenameMe_AllocateTotalMemory(V2MP_MemoryStoreRenameMe* mem, size_t sizeInBytes);

size_t V2MP_MemoryStoreRenameMe_GetTotalMemorySize(const V2MP_MemoryStoreRenameMe* mem);

V2MP_Byte* V2MP_MemoryStoreRenameMe_GetPtrToBase(V2MP_MemoryStoreRenameMe* mem);
const V2MP_Byte* V2MP_MemoryStoreRenameMe_GetConstPtrToBase(const V2MP_MemoryStoreRenameMe* mem);

bool V2MP_MemoryStoreRenameMe_LoadWord(
	const V2MP_MemoryStoreRenameMe* mem,
	V2MP_Word address,
	V2MP_Word* outWord
);

bool V2MP_MemoryStoreRenameMe_StoreWord(
	V2MP_MemoryStoreRenameMe* mem,
	V2MP_Word address,
	V2MP_Word inWord
);

// Returns null if the range would exceed the total memory area.
V2MP_Byte* V2MP_MemoryStoreRenameMe_GetPtrToRange(
	V2MP_MemoryStoreRenameMe* mem,
	size_t base,
	size_t length
);

// Returns null if the range would exceed the total memory area.
const V2MP_Byte* V2MP_MemoryStoreRenameMe_GetConstPtrToRange(
	const V2MP_MemoryStoreRenameMe* mem,
	size_t base,
	size_t length
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_MEMORYSTORE_H
