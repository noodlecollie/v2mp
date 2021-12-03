#ifndef V2MP_MEMORYSTORE_H
#define V2MP_MEMORYSTORE_H

#include <stdint.h>
#include <stdbool.h>
#include "V2MP/LibExport.h"
#include "V2MP/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _V2MP_Segment
{
	uint8_t* data;
	V2MP_Word sizeInBytes;
} V2MP_Segment;

typedef struct _V2MP_MemoryStore
{
	V2MP_Segment cs;
	V2MP_Segment ds;
} V2MP_MemoryStore;

API_V2MP void V2MP_MemoryStore_Init(V2MP_MemoryStore* mem);
API_V2MP void V2MP_MemoryStore_Deinit(V2MP_MemoryStore* mem);

API_V2MP bool V2MP_MemoryStore_AllocateCS(V2MP_MemoryStore* mem, V2MP_Word numWords, const V2MP_Word* data);
API_V2MP void V2MP_MemoryStore_FreeCS(V2MP_MemoryStore* mem);

API_V2MP bool V2MP_MemoryStore_FetchCSWord(
	const V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word* outWord,
	V2MP_Fault* outFault
);

API_V2MP bool V2MP_MemoryStore_AllocateDS(V2MP_MemoryStore* mem, V2MP_Word numWords, const V2MP_Word* data);
API_V2MP void V2MP_MemoryStore_FreeDS(V2MP_MemoryStore* mem);

API_V2MP bool V2MP_MemoryStore_FetchDSWord(
	const V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word* outWord,
	V2MP_Fault* outFault
);

API_V2MP bool V2MP_MemoryStore_StoreDSWord(
	V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word word,
	V2MP_Fault* outFault
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MEMORYSTORE_H
