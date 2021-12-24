#include "DataStructures/CircularBuffer.h"
#include "Util/Util.h"

struct V2MP_CircularBuffer
{
	V2MP_Byte* buffer;
	size_t bufferSize;
	V2MP_Byte* head;
	V2MP_Byte* tail;
};

#define CB_END(cb) ((cb)->buffer + (cb)->bufferSize)

V2MP_CircularBuffer* V2MP_CircularBuffer_AllocateAndInit(size_t capacity)
{
	V2MP_CircularBuffer* cb;

	if ( capacity < 1 || capacity == (size_t)(~0) )
	{
		return NULL;
	}

	cb = V2MP_CALLOC_STRUCT(V2MP_CircularBuffer);

	if ( !cb )
	{
		return NULL;
	}

	cb->bufferSize = capacity + 1;
	cb->buffer = (V2MP_Byte*)malloc(cb->bufferSize);

	if ( !cb->buffer )
	{
		free(cb);
		return NULL;
	}

	V2MP_CircularBuffer_Reset(cb);
	return cb;
}

void V2MP_CircularBuffer_DeinitAndFree(V2MP_CircularBuffer* cb)
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

void V2MP_CircularBuffer_Reset(V2MP_CircularBuffer* cb)
{
	if ( !cb )
	{
		return;
	}

	cb->head = cb->buffer;
	cb->tail = cb->buffer;
}

size_t V2MP_CircularBuffer_InternalSize(const V2MP_CircularBuffer* cb)
{
	return cb ? cb->bufferSize : 0;
}

size_t V2MP_CircularBuffer_Capacity(const V2MP_CircularBuffer* cb)
{
	return cb ? (cb->bufferSize - 1) : 0;
}

size_t V2MP_CircularBuffer_BytesFree(const V2MP_CircularBuffer* cb)
{
	if ( !cb )
	{
		return 0;
	}

	if ( cb->head >= cb->tail )
	{
		return V2MP_CircularBuffer_Capacity(cb) - (cb->head - cb->tail);
	}
	else
	{
		return cb->tail - cb->head - 1;
	}
}

size_t V2MP_CircularBuffer_BytesUsed(const V2MP_CircularBuffer* cb)
{
	return cb ? (V2MP_CircularBuffer_Capacity(cb) - V2MP_CircularBuffer_BytesFree(cb)) : 0;
}

bool V2MP_CircularBuffer_IsFull(const V2MP_CircularBuffer* cb)
{
	return cb ? V2MP_CircularBuffer_BytesFree(cb) == 0 : 0;
}

bool V2MP_CircularBuffer_IsEmpty(const V2MP_CircularBuffer* cb)
{
	return cb ? V2MP_CircularBuffer_BytesFree(cb) == V2MP_CircularBuffer_Capacity(cb) : 0;
}

const V2MP_Byte* V2MP_CircularBuffer_Head(const V2MP_CircularBuffer* cb)
{
	return cb ? cb->head : 0;
}

const V2MP_Byte* V2MP_CircularBuffer_Tail(const V2MP_CircularBuffer* cb)
{
	return cb ? cb->tail : 0;
}

size_t V2MP_CircularBuffer_WriteData(V2MP_CircularBuffer* cb, const V2MP_Byte* data, size_t dataSize)
{
	size_t bytesRead = 0;

	if ( !cb || !data || dataSize < 1 )
	{
		return 0;
	}

	while ( !V2MP_CircularBuffer_IsFull(cb) && bytesRead < dataSize )
	{
		const size_t bytesToCopy = V2MP_MIN((size_t)(CB_END(cb) - cb->head), dataSize - bytesRead);

		memcpy(cb->head, data + bytesRead, bytesToCopy);

		cb->head += bytesToCopy;
		bytesRead += bytesToCopy;

		if ( cb->head >= CB_END(cb) )
		{
			cb->head = cb->buffer;
		}
	}

	return bytesRead;
}

size_t V2MP_CircularBuffer_ReadData(V2MP_CircularBuffer* cb, V2MP_Byte* buffer, size_t bufferSize)
{
	size_t bytesWritten = 0;

	if ( !cb || !buffer || bufferSize < 1 )
	{
		return 0;
	}

	while ( !V2MP_CircularBuffer_IsEmpty(cb) && bytesWritten < bufferSize )
	{
		const size_t bytesToCopy = V2MP_MIN((size_t)(CB_END(cb) - cb->tail), bufferSize - bytesWritten);

		memcpy(buffer + bytesWritten, cb->tail, bytesToCopy);

		cb->tail += bytesToCopy;
		bytesWritten += bytesToCopy;

		if ( cb->tail >= CB_END(cb) )
		{
			cb->tail = cb->buffer;
		}
	}

	return bytesWritten;
}
