#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "V2MP/MemoryStore.h"
#include "V2MPInternal/Util/Util.h"
#include "V2MPInternal/Util/Heap.h"

typedef struct V2MP_Segment
{
	uint8_t* data;
	V2MP_Word sizeInBytes;
} V2MP_Segment;

struct V2MP_MemoryStore
{
	V2MP_Segment cs;
	V2MP_Segment ds;
};

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

	segment->data = (uint8_t*)V2MP_MALLOC(numWords * sizeof(V2MP_Word));

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

	if ( address & 0x1 )
	{
		if ( outFault )
		{
			*outFault = V2MP_FAULT_ALGN;
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

	if ( address & 0x1 )
	{
		if ( outFault )
		{
			*outFault = V2MP_FAULT_ALGN;
		}

		return;
	}

	*((V2MP_Word*)(segment->data + address)) = word;
}

static void WriteSegmentBytes(
	V2MP_Segment* segment,
	V2MP_Word address,
	const V2MP_Byte* data,
	size_t dataSize,
	size_t* numBytesWritten,
	V2MP_Fault* outFault
)
{
	if ( !segment->data )
	{
		if ( outFault )
		{
			*outFault = V2MP_FAULT_SEG;
		}

		return;
	}

	if ( (size_t)address + dataSize > segment->sizeInBytes )
	{
		if ( outFault )
		{
			*outFault = V2MP_FAULT_SEG;
		}

		// Write as many bytes as we can.
		dataSize = segment->sizeInBytes - address;
	}

	memcpy(segment->data + address, data, dataSize);

	if ( numBytesWritten )
	{
		*numBytesWritten = dataSize;
	}
}

size_t V2MP_MemoryStore_Footprint(void)
{
	return sizeof(V2MP_MemoryStore);
}

V2MP_MemoryStore* V2MP_MemoryStore_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_MemoryStore);
}

void V2MP_MemoryStore_DeinitAndFree(V2MP_MemoryStore* mem)
{
	if ( !mem )
	{
		return;
	}

	FreeSegment(&mem->cs);
	FreeSegment(&mem->ds);

	free(mem);
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

bool V2MP_MemoryStore_WriteBytesToDS(
	V2MP_MemoryStore* mem,
	V2MP_Word address,
	const V2MP_Byte* data,
	size_t dataSize,
	size_t* numBytesWritten,
	V2MP_Fault* outFault
)
{
	if ( numBytesWritten )
	{
		*numBytesWritten = 0;
	}

	if ( !mem || !data || dataSize < 1 )
	{
		return false;
	}

	WriteSegmentBytes(&mem->ds, address, data, dataSize, numBytesWritten, outFault);
	return true;
}
