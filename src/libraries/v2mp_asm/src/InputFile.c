#include <ctype.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"
#include "InputFile.h"

static void SetCurrentLine(V2MPAsm_InputFile* inputFile, size_t begin, size_t end)
{
	if ( !inputFile )
	{
		return;
	}

	if ( begin > inputFile->length )
	{
		begin = inputFile->length;
	}

	if ( end > inputFile->length )
	{
		end = inputFile->length;
	}

	if ( end < begin )
	{
		end = begin;
	}

	inputFile->curLineBegin = begin;
	inputFile->curLineEnd = end;
}

static inline void ResetCurrentLine(V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile )
	{
		return;
	}

	SetCurrentLine(inputFile, 0, 0);
	inputFile->curLineNo = 0;
}

V2MPAsm_InputFile* V2MPAsm_InputFile_AllocateAndInit(void)
{
	V2MPAsm_InputFile* inputFile;

	inputFile = BASEUTIL_CALLOC_STRUCT(V2MPAsm_InputFile);

	return inputFile;
}

void V2MPAsm_InputFile_DeinitAndFree(V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile )
	{
		return;
	}

	BASEUTIL_FREE(inputFile);
}

void V2MPAsm_InputFile_SetInput(V2MPAsm_InputFile* inputFile, const V2MPAsm_Byte* data, size_t length)
{
	if ( !inputFile )
	{
		return;
	}

	ResetCurrentLine(inputFile);

	if ( !data || length < 1 )
	{
		inputFile->data = NULL;
		inputFile->length = 0;

		return;
	}

	inputFile->data = data;
	inputFile->length = length;

	V2MPAsm_InputFile_SeekNextLine(inputFile);
}

bool V2MPAsm_InputFile_IsValid(const V2MPAsm_InputFile* inputFile)
{
	return inputFile && inputFile->data && inputFile->length > 0;
}

size_t V2MPAsm_InputFile_GetCurrentLineLength(const V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile || inputFile->curLineBegin > inputFile->curLineEnd )
	{
		return 0;
	}

	return inputFile->curLineEnd - inputFile->curLineBegin;
}

size_t V2MPAsm_InputFile_GetCurrentLineNumber(const V2MPAsm_InputFile* inputFile)
{
	return inputFile ? inputFile->curLineNo : 0;
}

void V2MPAsm_InputFile_SeekFirstLine(V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile )
	{
		return;
	}

	ResetCurrentLine(inputFile);
	V2MPAsm_InputFile_SeekNextLine(inputFile);
}

void V2MPAsm_InputFile_SeekNextLine(V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile )
	{
		return;
	}

	if ( !V2MPAsm_InputFile_IsValid(inputFile) )
	{
		ResetCurrentLine(inputFile);
		return;
	}

	if ( inputFile->curLineEnd >= inputFile->length )
	{
		// Seeking to EOF.
		SetCurrentLine(inputFile, inputFile->length, inputFile->length);
		++inputFile->curLineNo;

		return;
	}

	// Set the begin index to be where the end was previously.
	inputFile->curLineBegin = inputFile->curLineEnd;

	while ( inputFile->curLineEnd < inputFile->length && inputFile->data[inputFile->curLineEnd++] != '\n' )
	{
		// curLineEnd is incremented in the while condition.
	}

	// Now, the end index will either be one character in front of a newline, or one character in front
	// of the last character in the file (or both).

	++inputFile->curLineNo;
}

bool V2MPAsm_InputFile_IsEOF(const V2MPAsm_InputFile* inputFile)
{
	return inputFile ? inputFile->curLineBegin >= inputFile->length : true;
}

size_t V2MPAsm_InputFile_GetCurrentLineContent(const V2MPAsm_InputFile* inputFile, char* buffer, size_t maxLength)
{
	size_t lineLength;

	if ( !buffer || maxLength < 1 )
	{
		return 0;
	}

	buffer[0] = '\0';

	if ( !V2MPAsm_InputFile_IsValid(inputFile) || V2MPAsm_InputFile_IsEOF(inputFile) )
	{
		return 0;
	}

	lineLength = V2MPAsm_InputFile_GetCurrentLineLength(inputFile);
	lineLength = BASEUTIL_MIN(lineLength, maxLength - 1);

	if ( lineLength < 1 )
	{
		return 0;
	}

	memcpy(buffer, &inputFile->data[inputFile->curLineBegin], lineLength);
	buffer[lineLength] = '\0';

	return lineLength;
}
