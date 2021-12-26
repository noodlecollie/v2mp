#ifndef V2MPINTERNAL_CIRCULARBUFFER_H
#define V2MPINTERNAL_CIRCULARBUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Adapted from https://github.com/dhess/c-ringbuf

typedef struct V2MPI_CircularBuffer V2MPI_CircularBuffer;

V2MPI_CircularBuffer* V2MPI_CircularBuffer_AllocateAndInit(size_t capacity);
void V2MPI_CircularBuffer_DeinitAndFree(V2MPI_CircularBuffer* cb);

size_t V2MPI_CircularBuffer_Capacity(const V2MPI_CircularBuffer* cb);
size_t V2MPI_CircularBuffer_BytesFree(const V2MPI_CircularBuffer* cb);
size_t V2MPI_CircularBuffer_BytesUsed(const V2MPI_CircularBuffer* cb);
bool V2MPI_CircularBuffer_IsFull(const V2MPI_CircularBuffer* cb);
bool V2MPI_CircularBuffer_IsEmpty(const V2MPI_CircularBuffer* cb);

const uint8_t* V2MPI_CircularBuffer_Head(const V2MPI_CircularBuffer* cb);
const uint8_t* V2MPI_CircularBuffer_Tail(const V2MPI_CircularBuffer* cb);
size_t V2MPI_CircularBuffer_NumSequentialBytesReadableFromTail(const V2MPI_CircularBuffer* cb);

size_t V2MPI_CircularBuffer_WriteData(V2MPI_CircularBuffer* cb, const uint8_t* data, size_t dataSize);
size_t V2MPI_CircularBuffer_ReadData(V2MPI_CircularBuffer* cb, uint8_t* buffer, size_t bufferSize);
size_t V2MPI_CircularBuffer_DiscardBytes(V2MPI_CircularBuffer* cb, size_t numBytes);
void V2MPI_CircularBuffer_Reset(V2MPI_CircularBuffer* cb);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPINTERNAL_CIRCULARBUFFER_H
