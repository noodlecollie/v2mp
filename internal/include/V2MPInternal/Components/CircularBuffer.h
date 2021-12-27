#ifndef V2MP_CIRCULARBUFFER_H
#define V2MP_CIRCULARBUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Adapted from https://github.com/dhess/c-ringbuf

typedef struct V2MP_CircularBuffer V2MP_CircularBuffer;

V2MP_CircularBuffer* V2MP_CircularBuffer_AllocateAndInit(size_t capacity);
void V2MP_CircularBuffer_DeinitAndFree(V2MP_CircularBuffer* cb);

size_t V2MP_CircularBuffer_Capacity(const V2MP_CircularBuffer* cb);
size_t V2MP_CircularBuffer_BytesFree(const V2MP_CircularBuffer* cb);
size_t V2MP_CircularBuffer_BytesUsed(const V2MP_CircularBuffer* cb);
bool V2MP_CircularBuffer_IsFull(const V2MP_CircularBuffer* cb);
bool V2MP_CircularBuffer_IsEmpty(const V2MP_CircularBuffer* cb);

const uint8_t* V2MP_CircularBuffer_Head(const V2MP_CircularBuffer* cb);
const uint8_t* V2MP_CircularBuffer_Tail(const V2MP_CircularBuffer* cb);
size_t V2MP_CircularBuffer_NumSequentialBytesReadableFromTail(const V2MP_CircularBuffer* cb);

size_t V2MP_CircularBuffer_WriteData(V2MP_CircularBuffer* cb, const uint8_t* data, size_t dataSize);
size_t V2MP_CircularBuffer_ReadData(V2MP_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize);
size_t V2MP_CircularBuffer_DiscardBytes(V2MP_CircularBuffer* cb, size_t numBytes);
void V2MP_CircularBuffer_Reset(V2MP_CircularBuffer* cb);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_CIRCULARBUFFER_H
