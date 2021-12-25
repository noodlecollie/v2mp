#include "V2MPInternal/CircularBuffer.h"
#include "V2MPInternal/Util/Util.h"

struct V2MPCpts_CircularBuffer
{
	uint8_t* buffer;
	size_t bufferSize;
	uint8_t* head;
	uint8_t* tail;
};

#define CB_END(cb) ((cb)->buffer + (cb)->bufferSize - 1)

V2MPCpts_CircularBuffer* V2MPCpts_CircularBuffer_AllocateAndInit(size_t capacity)
{
	V2MPCpts_CircularBuffer* cb;

	if ( capacity < 1 || capacity == (size_t)(~0) )
	{
		return NULL;
	}

	cb = V2MP_CALLOC_STRUCT(V2MPCpts_CircularBuffer);

	if ( !cb )
	{
		return NULL;
	}

	cb->bufferSize = capacity + 1;
	cb->buffer = (uint8_t*)malloc(cb->bufferSize);

	if ( !cb->buffer )
	{
		free(cb);
		return NULL;
	}

	V2MPCpts_CircularBuffer_Reset(cb);
	return cb;
}

void V2MPCpts_CircularBuffer_DeinitAndFree(V2MPCpts_CircularBuffer* cb)
{
	if ( !cb )
	{
		return;
	}

	if ( cb->buffer )
	{
		free(cb->buffer);
	}

	free(cb);
}

size_t V2MPCpts_CircularBuffer_InternalSize(const V2MPCpts_CircularBuffer* cb)
{
	return cb ? cb->bufferSize : 0;
}

size_t V2MPCpts_CircularBuffer_Capacity(const V2MPCpts_CircularBuffer* cb)
{
	return cb ? (cb->bufferSize - 1) : 0;
}

size_t V2MPCpts_CircularBuffer_BytesFree(const V2MPCpts_CircularBuffer* cb)
{
	if ( !cb )
	{
		return 0;
	}

	if ( cb->head >= cb->tail )
	{
		return V2MPCpts_CircularBuffer_Capacity(cb) - (cb->head - cb->tail);
	}
	else
	{
		return cb->tail - cb->head - 1;
	}
}

size_t V2MPCpts_CircularBuffer_BytesUsed(const V2MPCpts_CircularBuffer* cb)
{
	return cb ? (V2MPCpts_CircularBuffer_Capacity(cb) - V2MPCpts_CircularBuffer_BytesFree(cb)) : 0;
}

bool V2MPCpts_CircularBuffer_IsFull(const V2MPCpts_CircularBuffer* cb)
{
	return cb ? V2MPCpts_CircularBuffer_BytesFree(cb) == 0 : 0;
}

bool V2MPCpts_CircularBuffer_IsEmpty(const V2MPCpts_CircularBuffer* cb)
{
	return cb ? V2MPCpts_CircularBuffer_BytesFree(cb) == V2MPCpts_CircularBuffer_Capacity(cb) : 0;
}

const uint8_t* V2MPCpts_CircularBuffer_Head(const V2MPCpts_CircularBuffer* cb)
{
	return cb ? cb->head : 0;
}

const uint8_t* V2MPCpts_CircularBuffer_Tail(const V2MPCpts_CircularBuffer* cb)
{
	return cb ? cb->tail : 0;
}

size_t V2MPCpts_CircularBuffer_WriteData(V2MPCpts_CircularBuffer* cb, const uint8_t* data, size_t dataSize)
{
	size_t bytesRead = 0;

	if ( !cb || !data || dataSize < 1 )
	{
		return 0;
	}

	while ( !V2MPCpts_CircularBuffer_IsFull(cb) && bytesRead < dataSize )
	{
		size_t bytesToCopy;

		if ( cb->head >= CB_END(cb) )
		{
			cb->head = cb->buffer;
		}

		bytesToCopy = V2MP_MIN((size_t)(CB_END(cb) - cb->head), dataSize - bytesRead);

		memcpy(cb->head, data + bytesRead, bytesToCopy);

		cb->head += bytesToCopy;
		bytesRead += bytesToCopy;
	}

	return bytesRead;
}

size_t V2MPCpts_CircularBuffer_ReadData(V2MPCpts_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize)
{
	size_t bytesWritten = 0;

	if ( !cb || !buffer || bufferSize < 1 )
	{
		return 0;
	}

	while ( !V2MPCpts_CircularBuffer_IsEmpty(cb) && bytesWritten < bufferSize )
	{
		size_t bytesToCopy;

		if ( cb->tail >= CB_END(cb) )
		{
			cb->tail = cb->buffer;
		}

		bytesToCopy = V2MP_MIN((size_t)(CB_END(cb) - cb->tail), bufferSize - bytesWritten);

		memcpy(buffer + bytesWritten, cb->tail, bytesToCopy);

		cb->tail += bytesToCopy;
		bytesWritten += bytesToCopy;
	}

	return bytesWritten;
}

void V2MPCpts_CircularBuffer_Reset(V2MPCpts_CircularBuffer* cb)
{
	if ( !cb )
	{
		return;
	}

	cb->head = cb->buffer;
	cb->tail = cb->buffer;
}
