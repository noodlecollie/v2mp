#include <ctype.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"
#include "InputFile.h"

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

	V2MPAsm_InputFile_ResetCurrentLine(inputFile);

	if ( !data || length < 1 )
	{
		inputFile->data = NULL;
		inputFile->length = 0;

		return;
	}

	inputFile->data = data;
	inputFile->length = length;
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
	lineLength = BASEUTIL_MIN(lineLength, maxLength - 1);

	if ( lineLength < 1 )
	{
		return 0;
	}

	memcpy(buffer, &inputFile->data[inputFile->curLineBegin], lineLength);
	buffer[lineLength] = '\0';

	return lineLength;
}
