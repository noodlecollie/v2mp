#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "V2MPAsm/TempTest.h"
#include "ParseContext.h"

static void ReadEachLine(V2MPAsm_ParseContext* context)
{
	V2MPAsm_InputFile* inputFile;
	char buffer[512];

	printf("Parsing: %s\n", V2MPAsm_ParseContext_GetFileName(context));

	inputFile = V2MPAsm_ParseContext_GetInputFile(context);

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

static void ParseFileData(const char* filePath, const V2MPAsm_Byte* buffer, size_t length)
{
	V2MPAsm_ParseContext* context = V2MPAsm_ParseContext_AllocateAndInit();

	if ( !context )
	{
		printf("Failed to create V2MPAsm_ParseContext\n");
		return;
	}

	V2MPAsm_ParseContext_SetInput(context, filePath, buffer, length);
	ReadEachLine(context);

	V2MPAsm_ParseContext_DeinitAndFree(context);
}

static void ReadFileFromDisk(const char* filePath, FILE* inFile)
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
	ParseFileData(filePath, buffer, length);

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
	ReadFileFromDisk(filePath, inFile);

	fclose(inFile);
}
