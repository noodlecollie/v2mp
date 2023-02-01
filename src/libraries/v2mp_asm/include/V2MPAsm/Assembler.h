#ifndef V2MPASM_PARSER_H
#define V2MPASM_PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include "V2MPAsm/LibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

struct V2MPAsm_Assembler;
struct V2MPAsm_Exception;

API_V2MPASM struct V2MPAsm_Assembler* V2MPAsm_Assembler_CreateFromFiles(const char* inputFile, const char* outputFile);
API_V2MPASM struct V2MPAsm_Assembler* V2MPAsm_Assembler_CreateFromMemory(const char* inputBuffer, const char* inputFileName);
API_V2MPASM void V2MPAsm_Assembler_Destroy(struct V2MPAsm_Assembler* assembler);

API_V2MPASM bool V2MPAsm_Assembler_Run(struct V2MPAsm_Assembler* assembler);
API_V2MPASM size_t V2MPAsm_Assembler_GetExceptionCount(const struct V2MPAsm_Assembler* assembler);
API_V2MPASM const struct V2MPAsm_Exception* V2MPAsm_Assembler_GetException(const struct V2MPAsm_Assembler* assembler, size_t index);

API_V2MPASM bool V2MPAsm_Assembler_HasInMemoryOutputBuffer(const struct V2MPAsm_Assembler* assembler);

// Required outBufferSize to be valid! If it is not, null will be returned.
// If V2MPAsm_Assembler_HasInMemoryOutputBuffer() is false, null will be returned.
API_V2MPASM void* V2MPAsm_Assembler_TakeInMemoryOutputBuffer(struct V2MPAsm_Assembler* assembler, size_t* outBufferSize);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_PARSER_H
