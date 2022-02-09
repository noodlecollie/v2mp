#ifndef V2MPASM_INPUTFILE_H
#define V2MPASM_INPUTFILE_H

#include <stddef.h>
#include <stdbool.h>
#include "Defs.h"

typedef struct V2MPAsm_InputFile
{
	const V2MPAsm_Byte* data;
	size_t length;

	size_t curLineNo;
	size_t curLineBegin;
	size_t curLineEnd;
} V2MPAsm_InputFile;

V2MPAsm_InputFile* V2MPAsm_InputFile_AllocateAndInit(void);
void V2MPAsm_InputFile_DeinitAndFree(V2MPAsm_InputFile* inputFile);

// The data is not owned by the input file.
void V2MPAsm_InputFile_SetInput(V2MPAsm_InputFile* inputFile, const V2MPAsm_Byte* data, size_t length);
bool V2MPAsm_InputFile_IsValid(const V2MPAsm_InputFile* inputFile);

size_t V2MPAsm_InputFile_GetCurrentLineLength(const V2MPAsm_InputFile* inputFile);
size_t V2MPAsm_InputFile_GetCurrentLineNumber(const V2MPAsm_InputFile* inputFile);
void V2MPAsm_InputFile_SeekFirstLine(V2MPAsm_InputFile* inputFile);
void V2MPAsm_InputFile_SeekNextLine(V2MPAsm_InputFile* inputFile);
bool V2MPAsm_InputFile_IsEOF(const V2MPAsm_InputFile* inputFile);

// Returns the number of bytes obtained, not including the terminator.
size_t V2MPAsm_InputFile_GetCurrentLineContent(const V2MPAsm_InputFile* inputFile, char* buffer, size_t maxLength);

#endif // V2MPASM_INPUTFILE_H
