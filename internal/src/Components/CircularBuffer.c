#include "V2MPInternal/Components/CircularBuffer.h"
#include "V2MPInternal/Util/Util.h"
#include "V2MPInternal/Util/Heap.h"

struct V2MP_CircularBuffer
{
	uint8_t* buffer;
	size_t capacity;
	size_t head;
	size_t tail;
	bool notEmpty;
};

static inline size_t MaxSequentialBytesWriteableToHead(const V2MP_CircularBuffer* cb)
{
	if ( cb->head > cb->tail )
	{
		// We can write all the way from the current head to the last index in the buffer.
		return cb->capacity - cb->head;
	}
	else if ( cb->tail > cb->head )
	{
		// We can write all the way from the current head to the index before the tail.
		return cb->tail - cb->head;
	}
	else
	{
		// If the buffer is not empty, it must be completely full,
		// in which case there is no more empty space to write to.
		// Otherwise, it must be empty, so we can write all the way
		// to the last index in the buffer.
		return cb->notEmpty ? 0 : (cb->capacity - cb->head);
	}
}

V2MP_CircularBuffer* V2MP_CircularBuffer_AllocateAndInit(size_t capacity)
{
	V2MP_CircularBuffer* cb;

	if ( capacity < 1 )
	{
		return NULL;
	}

	cb = V2MP_CALLOC_STRUCT(V2MP_CircularBuffer);

	if ( !cb )
	{
		return NULL;
	}

	cb->capacity = capacity;
	cb->buffer = (uint8_t*)V2MP_MALLOC(cb->capacity);

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

size_t V2MP_CircularBuffer_Capacity(const V2MP_CircularBuffer* cb)
{
	return cb ? cb->capacity : 0;
}

size_t V2MP_CircularBuffer_BytesFree(const V2MP_CircularBuffer* cb)
{
	return cb ? (cb->capacity - V2MP_CircularBuffer_BytesUsed(cb)) : 0;
}

size_t V2MP_CircularBuffer_BytesUsed(const V2MP_CircularBuffer* cb)
{
	if ( !cb )
	{
		return 0;
	}

	if ( cb->head > cb->tail )
	{
		return cb->head - cb->tail;
	}
	else if ( cb->tail > cb->head )
	{
		return cb->capacity - (cb->tail - cb->head);
	}
	else
	{
		return cb->notEmpty ? cb->capacity : 0;
	}
}

bool V2MP_CircularBuffer_IsFull(const V2MP_CircularBuffer* cb)
{
	return cb ? (cb->head == cb->tail && cb->notEmpty) : false;
}

bool V2MP_CircularBuffer_IsEmpty(const V2MP_CircularBuffer* cb)
{
	return cb ? !cb->notEmpty : true;
}

const uint8_t* V2MP_CircularBuffer_Head(const V2MP_CircularBuffer* cb)
{
	if ( !cb || V2MP_CircularBuffer_IsEmpty(cb) )
	{
		return NULL;
	}

	return &cb->buffer[cb->head];
}

const uint8_t* V2MP_CircularBuffer_Tail(const V2MP_CircularBuffer* cb)
{
	if ( !cb || V2MP_CircularBuffer_IsEmpty(cb) )
	{
		return NULL;
	}

	return &cb->buffer[cb->tail];
}

size_t V2MP_CircularBuffer_WriteData(V2MP_CircularBuffer* cb, const uint8_t* data, size_t dataSize)
{
	size_t bytesFree;
	size_t bytesToWrite;

	if ( !cb || !data || dataSize < 1 )
	{
		return 0;
	}

	bytesFree = V2MP_CircularBuffer_BytesFree(cb);

	if ( dataSize > bytesFree )
	{
		dataSize = bytesFree;
	}

	bytesToWrite = dataSize;

	while ( dataSize > 0 )
	{
		size_t numSequentialBytes;

		numSequentialBytes = MaxSequentialBytesWriteableToHead(cb);

		if ( numSequentialBytes > dataSize )
		{
			numSequentialBytes = dataSize;
		}

		memcpy(&cb->buffer[cb->head], data, numSequentialBytes);

		data += numSequentialBytes;
		dataSize -= numSequentialBytes;

		cb->head = (cb->head + numSequentialBytes) % cb->capacity;
	}

	if ( bytesToWrite > 0 )
	{
		cb->notEmpty = true;
	}

	return bytesToWrite;
}

size_t V2MP_CircularBuffer_ReadData(V2MP_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize)
{
	size_t bytesWaiting;
	size_t bytesRead;

	if ( !cb || !buffer || bufferSize < 1 )
	{
		return 0;
	}

	bytesWaiting = V2MP_CircularBuffer_BytesUsed(cb);

	if ( bytesWaiting > bufferSize )
	{
		bytesWaiting = bufferSize;
	}

	bytesRead = bytesWaiting;

	while ( bytesWaiting > 0 )
	{
		size_t numSequentialBytes;

		numSequentialBytes = V2MP_CircularBuffer_NumSequentialBytesReadableFromTail(cb);

		if ( numSequentialBytes > bytesWaiting )
		{
			numSequentialBytes = bytesWaiting;
		}

		memcpy(buffer, &cb->buffer[cb->tail], numSequentialBytes);

		buffer += numSequentialBytes;
		bytesWaiting -= numSequentialBytes;

		cb->tail = (cb->tail + numSequentialBytes) % cb->capacity;
	}

	if ( cb->head == cb->tail )
	{
		cb->notEmpty = false;
	}

	return bytesRead;
}

size_t V2MP_CircularBuffer_DiscardBytes(V2MP_CircularBuffer* cb, size_t numBytes)
{
	size_t bytesUsed;
	size_t bytesDiscarded;

	if ( !cb || numBytes < 1 )
	{
		return 0;
	}

	bytesUsed = V2MP_CircularBuffer_BytesUsed(cb);

	if ( numBytes > bytesUsed )
	{
		numBytes = bytesUsed;
	}

	bytesDiscarded = numBytes;

	while ( numBytes > 0 )
	{
		size_t numSequentialBytes;

		numSequentialBytes = V2MP_CircularBuffer_NumSequentialBytesReadableFromTail(cb);

		if ( numSequentialBytes > numBytes )
		{
			numSequentialBytes = numBytes;
		}

		numBytes -= numSequentialBytes;
		cb->tail = (cb->tail + numSequentialBytes) % cb->capacity;
	}

	return bytesDiscarded;
}

void V2MP_CircularBuffer_Reset(V2MP_CircularBuffer* cb)
{
	if ( !cb )
	{
		return;
	}

	cb->head = 0;
	cb->tail = 0;
	cb->notEmpty = false;
}

size_t V2MP_CircularBuffer_NumSequentialBytesReadableFromTail(const V2MP_CircularBuffer* cb)
{
	if ( !cb )
	{
		return 0;
	}

	if ( cb->tail > cb->head )
	{
		// We can read all the way from the current tail to the last index in the buffer.
		return cb->capacity - cb->tail;
	}
	else if ( cb->head > cb->tail )
	{
		// We can write all the way from the current tail to the index before the head.
		return cb->head - cb->tail;
	}
	else
	{
		// If the buffer is not empty, it must be completely full,
		// in which case we can read all the way to the last index
		// in the buffer. Otherwise the buffer is empty, so there
		// is no data to read.
		return cb->notEmpty ? (cb->capacity - cb->tail) : 0;
	}
}
