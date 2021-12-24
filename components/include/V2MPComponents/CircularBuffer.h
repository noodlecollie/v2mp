#ifndef V2MPCOMPONENTS_CIRCULARBUFFER_H
#define V2MPCOMPONENTS_CIRCULARBUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Adapted from https://github.com/dhess/c-ringbuf

typedef struct V2MPCpts_CircularBuffer V2MPCpts_CircularBuffer;

V2MPCpts_CircularBuffer* V2MPCpts_CircularBuffer_AllocateAndInit(size_t capacity);
void V2MPCpts_CircularBuffer_DeinitAndFree(V2MPCpts_CircularBuffer* cb);
void V2MPCpts_CircularBuffer_Reset(V2MPCpts_CircularBuffer* cb);

size_t V2MPCpts_CircularBuffer_InternalSize(const V2MPCpts_CircularBuffer* cb);
size_t V2MPCpts_CircularBuffer_Capacity(const V2MPCpts_CircularBuffer* cb);

size_t V2MPCpts_CircularBuffer_BytesFree(const V2MPCpts_CircularBuffer* cb);
size_t V2MPCpts_CircularBuffer_BytesUsed(const V2MPCpts_CircularBuffer* cb);
bool V2MPCpts_CircularBuffer_IsFull(const V2MPCpts_CircularBuffer* cb);
bool V2MPCpts_CircularBuffer_IsEmpty(const V2MPCpts_CircularBuffer* cb);

const uint8_t* V2MPCpts_CircularBuffer_Head(const V2MPCpts_CircularBuffer* cb);
const uint8_t* V2MPCpts_CircularBuffer_Tail(const V2MPCpts_CircularBuffer* cb);

size_t V2MPCpts_CircularBuffer_WriteData(V2MPCpts_CircularBuffer* cb, const uint8_t* data, size_t dataSize);
size_t V2MPCpts_CircularBuffer_ReadData(V2MPCpts_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPCOMPONENTS_CIRCULARBUFFER_H
