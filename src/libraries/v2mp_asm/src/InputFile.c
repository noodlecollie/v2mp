#include <ctype.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"
#include "BaseUtil/String.h"
#include "InputFile.h"

static void FindBeginningOfLineFromCurrentCursor(V2MPAsm_InputFile* inputFile)
{
	inputFile->beginningOfLine = inputFile->cursor;

	while ( inputFile->beginningOfLine > inputFile->data )
	{
		if ( *(inputFile->beginningOfLine - 1) == '\n' )
		{
			break;
		}

		--inputFile->beginningOfLine;
	}
}

static void WindCursorForward(V2MPAsm_InputFile* inputFile, const char* newCursor)
{
	while ( *inputFile->cursor && inputFile->cursor < newCursor )
	{
		if ( *(inputFile->cursor++) == '\n' )
		{
			inputFile->beginningOfLine = inputFile->cursor;
			++inputFile->curLineNo;
		}
	}
}

static void WindCursorBackward(V2MPAsm_InputFile* inputFile, const char* newCursor)
{
	while ( inputFile->cursor > newCursor )
	{
		if ( inputFile->cursor == inputFile->data )
		{
			break;
		}

		if ( *(--inputFile->cursor) == '\n' )
		{
			--inputFile->curLineNo;
			FindBeginningOfLineFromCurrentCursor(inputFile);
		}
	}
}

V2MPAsm_InputFile* V2MPAsm_InputFile_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPAsm_InputFile);
}

void V2MPAsm_InputFile_DeinitAndFree(V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile )
	{
		return;
	}

	BASEUTIL_FREE(inputFile);
}

void V2MPAsm_InputFile_SetInput(V2MPAsm_InputFile* inputFile, const char* data, size_t length)
{
	if ( !inputFile )
	{
		return;
	}

	if ( !data || length < 1 )
	{
		inputFile->data = NULL;
		inputFile->length = 0;
		inputFile->cursor = NULL;
		inputFile->beginningOfLine = NULL;
		inputFile->curLineNo = 0;

		return;
	}

	inputFile->data = data;
	inputFile->length = length;
	inputFile->cursor = data;
	inputFile->beginningOfLine = data;
	inputFile->curLineNo = 1;
}

bool V2MPAsm_InputFile_IsValid(const V2MPAsm_InputFile* inputFile)
{
	return inputFile && inputFile->data && inputFile->length > 0;
}

size_t V2MPAsm_InputFile_GetCurrentLineNumber(const V2MPAsm_InputFile* inputFile)
{
	return inputFile ? inputFile->curLineNo : 0;
}

size_t V2MPAsm_InputFile_GetCurrentColumnNumber(const V2MPAsm_InputFile* inputFile)
{
	return (inputFile && inputFile->cursor && inputFile->beginningOfLine)
		? (inputFile->cursor - inputFile->beginningOfLine + 1)
		: 0;
}

const char* V2MPAsm_InputFile_GetCursor(const V2MPAsm_InputFile* inputFile)
{
	return inputFile ? inputFile->cursor : NULL;
}

bool V2MPAsm_InputFile_IsEOF(const V2MPAsm_InputFile* inputFile)
{
	return !inputFile || !inputFile->cursor || !(*inputFile->cursor);
}

void V2MPAsm_InputFile_SkipWhitespace(V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile )
	{
		return;
	}

	V2MPAsm_InputFile_SkipToCursor(inputFile, BaseUtil_String_BeginWithoutWhitespace(inputFile->cursor));
}

void V2MPAsm_InputFile_SkipToCursor(V2MPAsm_InputFile* inputFile, const char* newCursor)
{
	if ( !V2MPAsm_InputFile_IsValid(inputFile) || !newCursor || newCursor == inputFile->cursor )
	{
		return;
	}

	if ( newCursor < inputFile->data )
	{
		inputFile->cursor = inputFile->data;
		inputFile->beginningOfLine = inputFile->cursor;
		inputFile->curLineNo = 0;

		return;
	}

	if ( newCursor > inputFile->cursor )
	{
		WindCursorForward(inputFile, newCursor);
	}
	else
	{
		WindCursorBackward(inputFile, newCursor);
	}
}

void V2MPAsm_InputFile_SkipToNextLine(V2MPAsm_InputFile* inputFile)
{
	size_t currentLine;

	if ( !V2MPAsm_InputFile_IsValid(inputFile) )
	{
		return;
	}

	currentLine = inputFile->curLineNo;
	V2MPAsm_InputFile_SkipWhitespace(inputFile);

	while ( inputFile->curLineNo == currentLine )
	{
		const char* cursor = inputFile->cursor;

		// Skip the current token
		while ( *cursor && !isspace(*cursor) )
		{
			++cursor;
		}

		// Skip any subsequent whitespace
		while ( *cursor && isspace(*cursor) )
		{
			++cursor;
		}

		// Move the actual cursor on to this position
		V2MPAsm_InputFile_SkipToCursor(inputFile, cursor);
	}
}
