#include "SharedComponents/CircularBuffer.h"
#include "BaseUtil/Util.h"
#include "BaseUtil/Heap.h"

typedef struct BufferRange
{
	size_t head;
	size_t tail;
	size_t notEmpty;
} BufferRange;

struct V2MPSC_CircularBuffer
{
	uint8_t* buffer;
	size_t capacity;
	BufferRange r;
};

static inline size_t MaxSequentialBytesWriteableToHead(const V2MPSC_CircularBuffer* cb)
{
	if ( cb->r.head > cb->r.tail )
	{
		// We can write all the way from the current head to the last index in the buffer.
		return cb->capacity - cb->r.head;
	}
	else if ( cb->r.tail > cb->r.head )
	{
		// We can write all the way from the current head to the index before the tail.
		return cb->r.tail - cb->r.head;
	}
	else
	{
		// If the buffer is not empty, it must be completely full,
		// in which case there is no more empty space to write to.
		// Otherwise, it must be empty, so we can write all the way
		// to the last index in the buffer.
		return cb->r.notEmpty ? 0 : (cb->capacity - cb->r.head);
	}
}

static inline size_t NumSequentialBytesReadableFromTail(const V2MPSC_CircularBuffer* cb, const BufferRange* range)
{
	if ( !cb )
	{
		return 0;
	}

	if ( !range )
	{
		range = &cb->r;
	}

	if ( range->tail > range->head )
	{
		// We can read all the way from the current tail to the last index in the buffer.
		return cb->capacity - range->tail;
	}
	else if ( range->head > range->tail )
	{
		// We can write all the way from the current tail to the index before the head.
		return range->head - range->tail;
	}
	else
	{
		// If the buffer is not empty, it must be completely full,
		// in which case we can read all the way to the last index
		// in the buffer. Otherwise the buffer is empty, so there
		// is no data to read.
		return range->notEmpty ? (cb->capacity - range->tail) : 0;
	}
}

static size_t ReadData(const V2MPSC_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize, BufferRange* range)
{
	size_t bytesWaiting;
	size_t bytesRead;

	if ( !cb || !range )
	{
		return 0;
	}

	// We only copy out to a buffer if it's valid.
	// Otherwise, we just discard bytes.
	if ( buffer && bufferSize < 1 )
	{
		buffer = NULL;
		bufferSize = 0;
	}

	bytesWaiting = V2MPSC_CircularBuffer_BytesUsed(cb);

	if ( bufferSize > 0 && bytesWaiting > bufferSize )
	{
		bytesWaiting = bufferSize;
	}

	bytesRead = bytesWaiting;

	while ( bytesWaiting > 0 )
	{
		size_t numSequentialBytes;

		numSequentialBytes = V2MPSC_CircularBuffer_NumSequentialBytesReadableFromTail(cb);

		if ( numSequentialBytes > bytesWaiting )
		{
			numSequentialBytes = bytesWaiting;
		}

		if ( buffer )
		{
			memcpy(buffer, &cb->buffer[range->tail], numSequentialBytes);
			buffer += numSequentialBytes;
		}

		bytesWaiting -= numSequentialBytes;
		range->tail = (range->tail + numSequentialBytes) % cb->capacity;
	}

	if ( range->head == range->tail )
	{
		range->notEmpty = false;
	}

	return bytesRead;
}

V2MPSC_CircularBuffer* V2MPSC_CircularBuffer_AllocateAndInit(size_t capacity)
{
	V2MPSC_CircularBuffer* cb;

	if ( capacity < 1 )
	{
		return NULL;
	}

	cb = BASEUTIL_CALLOC_STRUCT(V2MPSC_CircularBuffer);

	if ( !cb )
	{
		return NULL;
	}

	cb->capacity = capacity;
	cb->buffer = (uint8_t*)BASEUTIL_MALLOC(cb->capacity);

	if ( !cb->buffer )
	{
		free(cb);
		return NULL;
	}

	V2MPSC_CircularBuffer_Reset(cb);
	return cb;
}

void V2MPSC_CircularBuffer_DeinitAndFree(V2MPSC_CircularBuffer* cb)
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

size_t V2MPSC_CircularBuffer_Capacity(const V2MPSC_CircularBuffer* cb)
{
	return cb ? cb->capacity : 0;
}

size_t V2MPSC_CircularBuffer_BytesFree(const V2MPSC_CircularBuffer* cb)
{
	return cb ? (cb->capacity - V2MPSC_CircularBuffer_BytesUsed(cb)) : 0;
}

size_t V2MPSC_CircularBuffer_BytesUsed(const V2MPSC_CircularBuffer* cb)
{
	if ( !cb )
	{
		return 0;
	}

	if ( cb->r.head > cb->r.tail )
	{
		return cb->r.head - cb->r.tail;
	}
	else if ( cb->r.tail > cb->r.head )
	{
		return cb->capacity - (cb->r.tail - cb->r.head);
	}
	else
	{
		return cb->r.notEmpty ? cb->capacity : 0;
	}
}

bool V2MPSC_CircularBuffer_IsFull(const V2MPSC_CircularBuffer* cb)
{
	return cb ? (cb->r.head == cb->r.tail && cb->r.notEmpty) : false;
}

bool V2MPSC_CircularBuffer_IsEmpty(const V2MPSC_CircularBuffer* cb)
{
	return cb ? !cb->r.notEmpty : true;
}

const uint8_t* V2MPSC_CircularBuffer_Head(const V2MPSC_CircularBuffer* cb)
{
	if ( !cb || V2MPSC_CircularBuffer_IsEmpty(cb) )
	{
		return NULL;
	}

	return &cb->buffer[cb->r.head];
}

const uint8_t* V2MPSC_CircularBuffer_Tail(const V2MPSC_CircularBuffer* cb)
{
	if ( !cb || V2MPSC_CircularBuffer_IsEmpty(cb) )
	{
		return NULL;
	}

	return &cb->buffer[cb->r.tail];
}

size_t V2MPSC_CircularBuffer_WriteData(V2MPSC_CircularBuffer* cb, const uint8_t* data, size_t dataSize)
{
	size_t bytesFree;
	size_t bytesToWrite;

	if ( !cb || !data || dataSize < 1 )
	{
		return 0;
	}

	bytesFree = V2MPSC_CircularBuffer_BytesFree(cb);

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

		memcpy(&cb->buffer[cb->r.head], data, numSequentialBytes);

		data += numSequentialBytes;
		dataSize -= numSequentialBytes;

		cb->r.head = (cb->r.head + numSequentialBytes) % cb->capacity;
	}

	if ( bytesToWrite > 0 )
	{
		cb->r.notEmpty = true;
	}

	return bytesToWrite;
}

size_t V2MPSC_CircularBuffer_ReadData(V2MPSC_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize)
{
	if ( !cb || !buffer || bufferSize < 1 )
	{
		return 0;
	}

	return ReadData(cb, buffer, bufferSize, &cb->r);
}

size_t V2MPSC_CircularBuffer_DiscardBytes(V2MPSC_CircularBuffer* cb, size_t numBytes)
{
	if ( !cb )
	{
		return 0;
	}

	return ReadData(cb, NULL, numBytes, &cb->r);
}

void V2MPSC_CircularBuffer_Reset(V2MPSC_CircularBuffer* cb)
{
	if ( !cb )
	{
		return;
	}

	cb->r.head = 0;
	cb->r.tail = 0;
	cb->r.notEmpty = false;
}

size_t V2MPSC_CircularBuffer_NumSequentialBytesReadableFromTail(const V2MPSC_CircularBuffer* cb)
{
	if ( !cb )
	{
		return 0;
	}

	return NumSequentialBytesReadableFromTail(cb, NULL);
}

size_t V2MPSC_CircularBuffer_CopyData(V2MPSC_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize)
{
	BufferRange range;

	if ( !cb || !buffer || bufferSize < 1 )
	{
		return 0;
	}

	// Create a fake range struct to modify, leaving the circular buffer itself unchanged.
	range = cb->r;

	return ReadData(cb, buffer, bufferSize, &range);
}
