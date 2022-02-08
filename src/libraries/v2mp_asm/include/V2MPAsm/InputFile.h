#ifndef V2MPASM_INPUTFILE_H
#define V2MPASM_INPUTFILE_H

#include <stddef.h>
#include <stdbool.h>
#include "V2MPAsm/LibExport.h"
#include "V2MPAsm/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPAsm_InputFile V2MPAsm_InputFile;

API_V2MPASM V2MPAsm_InputFile* V2MPAsm_InputFile_AllocateAndInit(void);
API_V2MPASM void V2MPAsm_InputFile_DeinitAndFree(V2MPAsm_InputFile* inputFile);

// The data is not owned by the input file.
API_V2MPASM void V2MPAsm_InputFile_SetInput(V2MPAsm_InputFile* inputFile, const V2MPAsm_Byte* data, size_t length);
API_V2MPASM bool V2MPAsm_InputFile_IsValid(const V2MPAsm_InputFile* inputFile);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_INPUTFILE_H
