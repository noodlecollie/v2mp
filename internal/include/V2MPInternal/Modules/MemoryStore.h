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

// Returns null if the range would exceed the total memory area.
V2MP_Byte* V2MP_MemoryStoreRenameMe_GetPtrToRange(V2MP_MemoryStoreRenameMe* mem, size_t base, size_t length);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_MEMORYSTORE_H
