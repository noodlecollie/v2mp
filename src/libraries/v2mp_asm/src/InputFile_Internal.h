#ifndef V2MPASM_INPUTFILE_INTERNAL_H
#define V2MPASM_INPUTFILE_INTERNAL_H

#include <stdbool.h>
#include "V2MPAsm/InputFile.h"

struct V2MPAsm_InputFile
{
	const V2MPAsm_Byte* data;
	size_t length;

	size_t curLineBegin;
	size_t curLineEnd;
};

size_t V2MPAsm_InputFile_GetCurrentLineLength(const V2MPAsm_InputFile* inputFile);
void V2MPAsm_InputFile_SetCurrentLine(V2MPAsm_InputFile* inputFile, size_t begin, size_t end);
void V2MPAsm_InputFile_ResetCurrentLine(V2MPAsm_InputFile* inputFile);
bool V2MPAsm_InputFile_IsEOF(const V2MPAsm_InputFile* inputFile);

// Returns the number of bytes obtained, not including the terminator.
size_t V2MPAsm_InputFile_GetCurrentLineContent(const V2MPAsm_InputFile* inputFile, char* buffer, size_t maxLength);

#endif // V2MPASM_INPUTFILE_INTERNAL_H
