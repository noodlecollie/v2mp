#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "V2MPAsm/TempTest.h"
#include "InputFile.h"

static void ReadEachLine(V2MPAsm_InputFile* inputFile)
{
	char buffer[512];

	do
	{
		size_t lineNumber;
		size_t charsRead;
		size_t charsAfterTrimming;
		char* newTerminator;
		const char* newBegin;

		lineNumber = V2MPAsm_InputFile_GetCurrentLineNumber(inputFile);
		printf("Line %lu: ", lineNumber);

		if ( V2MPAsm_InputFile_IsEOF(inputFile) )
		{
			printf("EOF\n");
			break;
		}

		charsRead = V2MPAsm_InputFile_GetCurrentLineContent(inputFile, buffer, sizeof(buffer));

		newTerminator = (char*)BaseUtil_String_EndWithoutWhitespace(buffer);
		*newTerminator = '\0';

		newBegin = BaseUtil_String_BeginWithoutWhitespace(buffer);
		charsAfterTrimming = strlen(newBegin);

		if ( charsRead > 0 )
		{
			printf("%s (%lu characters read, trimmed to %lu characters)\n", newBegin, charsRead, charsAfterTrimming);
		}
		else
		{
			printf(" (0 characters, trimmed to 0 characters)\n");
		}

		V2MPAsm_InputFile_SeekNextLine(inputFile);
	}
	while ( true );
}

static void ParseFileData(const V2MPAsm_Byte* buffer, size_t length)
{
	V2MPAsm_InputFile* inputFile = V2MPAsm_InputFile_AllocateAndInit();

	if ( !inputFile )
	{
		printf("Failed to create V2MPAsm_InputFile\n");
		return;
	}

	V2MPAsm_InputFile_SetInput(inputFile, buffer, length);
	ReadEachLine(inputFile);

	V2MPAsm_InputFile_DeinitAndFree(inputFile);
}

static void ReadFileFromDisk(FILE* inFile)
{
	size_t length = 0;
	V2MPAsm_Byte* buffer;

	fseek(inFile, 0, SEEK_END);
	length = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	printf("File size: %lu bytes\n", length);

	buffer = BASEUTIL_MALLOC(length);

	if ( !buffer )
	{
		printf("Could not allocate buffer to read file.\n");
		return;
	}

	fread(buffer, 1, length, inFile);
	ParseFileData(buffer, length);

	BASEUTIL_FREE(buffer);
}

void V2MPAsm_TempTest_ReadFile(const char* filePath)
{
	FILE* inFile = NULL;

	if ( !filePath || !(*filePath) )
	{
		printf("No file path was provided.\n");
		return;
	}

	inFile = fopen(filePath, "r");

	if ( !inFile )
	{
		printf("Could not open file %s\n", filePath);
		return;
	}

	printf("Reading file %s\n", filePath);
	ReadFileFromDisk(inFile);

	fclose(inFile);
}
