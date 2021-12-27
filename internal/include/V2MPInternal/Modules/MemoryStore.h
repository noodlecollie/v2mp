#ifndef V2MP_MODULES_MEMORYSTORE_H
#define V2MP_MODULES_MEMORYSTORE_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_MemoryStoreRenameMe V2MP_MemoryStoreRenameMe;

V2MP_MemoryStoreRenameMe* V2MP_MemoryStoreRenameMe_AllocateAndInit(void);
void V2MP_MemoryStoreRenameMe_DeinitAndFree(V2MP_MemoryStoreRenameMe* mem);

// Total memory must be a multiple of sizeof(V2MP_Word), otherwise allocation will fail.
bool V2MP_MemoryStoreRenameMe_AllocateTotalMemory(V2MP_MemoryStoreRenameMe* mem, size_t sizeInBytes);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_MEMORYSTORE_H
