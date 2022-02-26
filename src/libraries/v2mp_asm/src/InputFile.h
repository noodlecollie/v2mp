#ifndef V2MPASM_INPUTFILE_H
#define V2MPASM_INPUTFILE_H

#include <stddef.h>
#include <stdbool.h>
#include "Defs.h"

typedef struct V2MPAsm_InputFile
{
	const char* data;
	size_t length;

	const char* cursor;
	const char* beginningOfLine;
	size_t curLineNo;
} V2MPAsm_InputFile;

V2MPAsm_InputFile* V2MPAsm_InputFile_AllocateAndInit(void);
void V2MPAsm_InputFile_DeinitAndFree(V2MPAsm_InputFile* inputFile);

// The data is not owned by the input file.
void V2MPAsm_InputFile_SetInput(V2MPAsm_InputFile* inputFile, const char* data, size_t length);
bool V2MPAsm_InputFile_IsValid(const V2MPAsm_InputFile* inputFile);

size_t V2MPAsm_InputFile_GetCurrentLineNumber(const V2MPAsm_InputFile* inputFile);
size_t V2MPAsm_InputFile_GetCurrentColumnNumber(const V2MPAsm_InputFile* inputFile);
const char* V2MPAsm_InputFile_GetCursor(const V2MPAsm_InputFile* inputFile);
bool V2MPAsm_InputFile_IsEOF(const V2MPAsm_InputFile* inputFile);

void V2MPAsm_InputFile_SkipWhitespace(V2MPAsm_InputFile* inputFile);
void V2MPAsm_InputFile_SkipToCursor(V2MPAsm_InputFile* inputFile, const char* newCursor);
void V2MPAsm_InputFile_SkipToNextLine(V2MPAsm_InputFile* inputFile);

const char* V2MPAsm_InputFile_FindEndOfCurrentLine(const V2MPAsm_InputFile* inputFile);

#endif // V2MPASM_INPUTFILE_H
