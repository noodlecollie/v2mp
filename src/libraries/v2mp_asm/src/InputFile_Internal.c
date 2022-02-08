#include <ctype.h>
#include "BaseUtil/Util.h"
#include "InputFile_Internal.h"

size_t V2MPAsm_InputFile_GetCurrentLineLength(const V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile || inputFile->curLineBegin > inputFile->curLineEnd )
	{
		return 0;
	}

	return inputFile->curLineEnd - inputFile->curLineBegin;
}

void V2MPAsm_InputFile_SetCurrentLine(V2MPAsm_InputFile* inputFile, size_t begin, size_t end)
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

void V2MPAsm_InputFile_ResetCurrentLine(V2MPAsm_InputFile* inputFile)
{
	V2MPAsm_InputFile_SetCurrentLine(inputFile, 0, 0);
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

	// Don't copy any trailing whitespace.
	while ( lineLength > 0 && isspace(inputFile->data[inputFile->curLineBegin + lineLength - 1]) )
	{
		--lineLength;
	}

	lineLength = BASEUTIL_MIN(lineLength, maxLength - 1);

	if ( lineLength < 1 )
	{
		return 0;
	}

	memcpy(buffer, &inputFile->data[inputFile->curLineBegin], lineLength);
	buffer[lineLength] = '\0';

	return lineLength;
}
