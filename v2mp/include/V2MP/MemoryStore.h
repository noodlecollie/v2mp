#ifndef V2MP_MEMORYSTORE_H
#define V2MP_MEMORYSTORE_H

#include <stdint.h>
#include <stdbool.h>
#include "V2MP/Defs.h"

typedef struct _V2MP_Segment
{
	uint8_t* data;
	V2MP_Word size;
} V2MP_Segment;

typedef struct _V2MP_MemoryStore
{
	V2MP_Segment cs;
	V2MP_Segment ds;
} V2MP_MemoryStore;

void V2MP_MemoryStore_Init(V2MP_MemoryStore* mem);
void V2MP_MemoryStore_Deinit(V2MP_MemoryStore* mem);

bool V2MP_MemoryStore_AllocateCS(V2MP_MemoryStore* mem, V2MP_Word size, const void* data);
void V2MP_MemoryStore_FreeCS(V2MP_MemoryStore* mem);

bool V2MP_MemoryStore_FetchCSWord(
	V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word* outWord,
	V2MP_Fault* outFault
);

bool V2MP_MemoryStore_AllocateDS(V2MP_MemoryStore* mem, V2MP_Word size, const void* data);
void V2MP_MemoryStore_FreeDS(V2MP_MemoryStore* mem);

bool V2MP_MemoryStore_FetchDSWord(
	V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word* outWord,
	V2MP_Fault* outFault
);

#endif // V2MP_MEMORYSTORE_H
