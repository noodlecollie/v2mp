#include "Parsing.h"

void V2MPAsm_Parsing_SeekNextLine(V2MPAsm_InputFile* inputFile)
{
	if ( !inputFile )
	{
		return;
	}

	if ( !V2MPAsm_InputFile_IsValid(inputFile) )
	{
		V2MPAsm_InputFile_ResetCurrentLine(inputFile);
		return;
	}

	if ( inputFile->curLineEnd >= inputFile->length )
	{
		V2MPAsm_InputFile_SetCurrentLine(inputFile, inputFile->length, inputFile->length);
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
}
