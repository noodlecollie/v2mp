#ifndef SHAREDCOMPONENTS_CIRCULARBUFFER_H
#define SHAREDCOMPONENTS_CIRCULARBUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Adapted from https://github.com/dhess/c-ringbuf

typedef struct V2MPSC_CircularBuffer V2MPSC_CircularBuffer;

V2MPSC_CircularBuffer* V2MPSC_CircularBuffer_AllocateAndInit(size_t capacity);
void V2MPSC_CircularBuffer_DeinitAndFree(V2MPSC_CircularBuffer* cb);

size_t V2MPSC_CircularBuffer_Capacity(const V2MPSC_CircularBuffer* cb);
size_t V2MPSC_CircularBuffer_BytesFree(const V2MPSC_CircularBuffer* cb);
size_t V2MPSC_CircularBuffer_BytesUsed(const V2MPSC_CircularBuffer* cb);
bool V2MPSC_CircularBuffer_IsFull(const V2MPSC_CircularBuffer* cb);
bool V2MPSC_CircularBuffer_IsEmpty(const V2MPSC_CircularBuffer* cb);

const uint8_t* V2MPSC_CircularBuffer_Head(const V2MPSC_CircularBuffer* cb);
const uint8_t* V2MPSC_CircularBuffer_Tail(const V2MPSC_CircularBuffer* cb);
size_t V2MPSC_CircularBuffer_NumSequentialBytesReadableFromTail(const V2MPSC_CircularBuffer* cb);

size_t V2MPSC_CircularBuffer_WriteData(V2MPSC_CircularBuffer* cb, const uint8_t* data, size_t dataSize);
size_t V2MPSC_CircularBuffer_ReadData(V2MPSC_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize);
size_t V2MPSC_CircularBuffer_DiscardBytes(V2MPSC_CircularBuffer* cb, size_t numBytes);
void V2MPSC_CircularBuffer_Reset(V2MPSC_CircularBuffer* cb);

// In contrast to ReadData(), this function does not consume the data from the buffer.
size_t V2MPSC_CircularBuffer_CopyData(V2MPSC_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SHAREDCOMPONENTS_CIRCULARBUFFER_H
