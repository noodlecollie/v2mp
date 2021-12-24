#ifndef V2MP_CIRCULARBUFFER_H
#define V2MP_CIRCULARBUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include "V2MP/Defs.h"

// Adapted from https://github.com/dhess/c-ringbuf

typedef struct V2MP_CircularBuffer V2MP_CircularBuffer;

V2MP_CircularBuffer* V2MP_CircularBuffer_AllocateAndInit(size_t capacity);
void V2MP_CircularBuffer_DeinitAndFree(V2MP_CircularBuffer* cb);
void V2MP_CircularBuffer_Reset(V2MP_CircularBuffer* cb);

size_t V2MP_CircularBuffer_InternalSize(const V2MP_CircularBuffer* cb);
size_t V2MP_CircularBuffer_Capacity(const V2MP_CircularBuffer* cb);

size_t V2MP_CircularBuffer_BytesFree(const V2MP_CircularBuffer* cb);
size_t V2MP_CircularBuffer_BytesUsed(const V2MP_CircularBuffer* cb);
bool V2MP_CircularBuffer_IsFull(const V2MP_CircularBuffer* cb);
bool V2MP_CircularBuffer_IsEmpty(const V2MP_CircularBuffer* cb);

const V2MP_Byte* V2MP_CircularBuffer_Head(const V2MP_CircularBuffer* cb);
const V2MP_Byte* V2MP_CircularBuffer_Tail(const V2MP_CircularBuffer* cb);

size_t V2MP_CircularBuffer_WriteData(V2MP_CircularBuffer* cb, const V2MP_Byte* data, size_t dataSize);
size_t V2MP_CircularBuffer_ReadData(V2MP_CircularBuffer* cb, V2MP_Byte* buffer, size_t bufferSize);

#endif // V2MP_CIRCULARBUFFER_H
