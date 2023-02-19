#ifndef V2MPASM_ASSEMBLER_H
#define V2MPASM_ASSEMBLER_H

#include <stddef.h>
#include <stdbool.h>
#include "LibV2MPAsm/LibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

struct V2MPAsm_Assembler;
struct V2MPAsm_Exception;

typedef enum V2MPAsm_AssemblerResult
{
	V2MPASM_COMPLETED_OK = 0,
	V2MPASM_COMPLETED_WITH_WARNINGS,
	V2MPASM_FAILED
} V2MPAsm_AssemblerResult;

API_LIBV2MPASM struct V2MPAsm_Assembler* V2MPAsm_Assembler_CreateFromFiles(const char* inputFile, const char* outputFile);
API_LIBV2MPASM struct V2MPAsm_Assembler* V2MPAsm_Assembler_CreateFromMemory(const char* inputFileName, const char* inputBuffer);
API_LIBV2MPASM void V2MPAsm_Assembler_Destroy(struct V2MPAsm_Assembler* assembler);

API_LIBV2MPASM V2MPAsm_AssemblerResult V2MPAsm_Assembler_Run(struct V2MPAsm_Assembler* assembler);
API_LIBV2MPASM size_t V2MPAsm_Assembler_GetExceptionCount(const struct V2MPAsm_Assembler* assembler);
API_LIBV2MPASM const struct V2MPAsm_Exception* V2MPAsm_Assembler_GetException(const struct V2MPAsm_Assembler* assembler, size_t index);

API_LIBV2MPASM bool V2MPAsm_Assembler_HasInMemoryOutputBuffer(const struct V2MPAsm_Assembler* assembler);
API_LIBV2MPASM size_t V2MPAsm_Assembler_InMemoryOutputBufferSizeInBytes(const struct V2MPAsm_Assembler* assembler);

// Returns the number of bytes that were written. Does not write off the
// end of the buffer if it was too small.
// If V2MPAsm_Assembler_HasInMemoryOutputBuffer() is false, nothing is
// written and 0 is returned.
// Otherwise, the data in the internal buffer is copied out (if the
// provided buffer and length are valid). The internal buffer is always
// left empty after this call.
// Any bytes in the output buffer that cannot contain an entire code word,
// or which exceed the size of the internal buffer being copied out, are
// not written to at all. Their values should be considered undefined.
API_LIBV2MPASM size_t V2MPAsm_Assembler_TakeInMemoryOutputBuffer(struct V2MPAsm_Assembler* assembler, void* outBuffer, size_t outBufferSizeInBytes);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_ASSEMBLER_H
