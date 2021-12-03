#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "V2MP/MemoryStore.h"

static void FreeSegment(V2MP_Segment* segment)
{
	if ( segment->data )
	{
		free(segment->data);
	}

	segment->data = NULL;
	segment->sizeInBytes = 0;
}

static bool AllocateSegment(V2MP_Segment* segment, V2MP_Word numWords, const V2MP_Word* data)
{
	FreeSegment(segment);

	if ( numWords < 1 )
	{
		return false;
	}

	segment->data = (uint8_t*)malloc(numWords * sizeof(V2MP_Word));

	if ( !segment->data )
	{
		return false;
	}

	segment->sizeInBytes = numWords * sizeof(V2MP_Word);

	if ( data )
	{
		memcpy(segment->data, data, segment->sizeInBytes);
	}
	else
	{
		memset(segment->data, 0, segment->sizeInBytes);
	}

	return true;
}

static void GetSegmentWord(
	const V2MP_Segment* segment,
	V2MP_Word address,
	V2MP_Word* outWord,
	V2MP_Fault* outFault
)
{
	if ( !segment->data || (size_t)address + sizeof(V2MP_Word) > segment->sizeInBytes )
	{
		if ( outFault )
		{
			*outFault = V2MP_FAULT_SEG;
		}

		return;
	}

	*outWord = *((const V2MP_Word*)(segment->data + address));
}

static void SetSegmentWord(
	V2MP_Segment* segment,
	V2MP_Word address,
	V2MP_Word word,
	V2MP_Fault* outFault
)
{
	if ( !segment->data || (size_t)address + sizeof(V2MP_Word) > segment->sizeInBytes )
	{
		if ( outFault )
		{
			*outFault = V2MP_FAULT_SEG;
		}

		return;
	}

	*((V2MP_Word*)(segment->data + address)) = word;
}

void V2MP_MemoryStore_Init(V2MP_MemoryStore* mem)
{
	if ( !mem )
	{
		return;
	}

	memset(mem, 0, sizeof(*mem));
}

void V2MP_MemoryStore_Deinit(V2MP_MemoryStore* mem)
{
	if ( !mem )
	{
		return;
	}

	FreeSegment(&mem->cs);
	FreeSegment(&mem->ds);
}

bool V2MP_MemoryStore_AllocateCS(V2MP_MemoryStore* mem, V2MP_Word numWords, const V2MP_Word* data)
{
	if ( !mem )
	{
		return false;
	}

	return AllocateSegment(&mem->cs, numWords, data);
}

void V2MP_MemoryStore_FreeCS(V2MP_MemoryStore* mem)
{
	V2MP_MemoryStore_AllocateCS(mem, 0, NULL);
}

bool V2MP_MemoryStore_FetchCSWord(
	const V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word* outWord,
	V2MP_Fault* outFault
)
{
	if ( !mem || !outWord )
	{
		return false;
	}

	GetSegmentWord(&mem->cs, address, outWord, outFault);
	return true;
}

bool V2MP_MemoryStore_AllocateDS(V2MP_MemoryStore* mem, V2MP_Word numWords, const V2MP_Word* data)
{
	if ( !mem )
	{
		return false;
	}

	return AllocateSegment(&mem->ds, numWords, data);
}

void V2MP_MemoryStore_FreeDS(V2MP_MemoryStore* mem)
{
	V2MP_MemoryStore_AllocateDS(mem, 0, NULL);
}

bool V2MP_MemoryStore_FetchDSWord(
	const V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word* outWord,
	V2MP_Fault* outFault
)
{
	if ( !mem || !outWord )
	{
		return false;
	}

	GetSegmentWord(&mem->ds, address, outWord, outFault);
	return true;
}

bool V2MP_MemoryStore_StoreDSWord(
	V2MP_MemoryStore* mem,
	V2MP_Word address,
	V2MP_Word word,
	V2MP_Fault* outFault
)
{
	if ( !mem )
	{
		return false;
	}

	SetSegmentWord(&mem->ds, address, word, outFault);
	return true;
}
