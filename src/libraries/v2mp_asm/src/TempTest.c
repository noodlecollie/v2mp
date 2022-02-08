#include <stdio.h>
#include "V2MPAsm/TempTest.h"
#include "V2MPAsm/InputFile.h"
#include "BaseUtil/Heap.h"
#include "InputFile_Internal.h"
#include "Parsing.h"

static void ReadEachLine(V2MPAsm_InputFile* inputFile)
{
	char buffer[512];
	size_t lineNumber = 1;

	do
	{
		size_t charsRead;

		V2MPAsm_Parsing_SeekNextLine(inputFile);

		printf("Line %lu: ", lineNumber);

		if ( V2MPAsm_InputFile_IsEOF(inputFile) )
		{
			printf("EOF\n");
			break;
		}

		charsRead = V2MPAsm_InputFile_GetCurrentLineContent(inputFile, buffer, sizeof(buffer));

		if ( charsRead > 0 )
		{
			printf("%s (%lu characters)\n", buffer, charsRead);
		}
		else
		{
			printf(" (0 characters)\n");
		}

		++lineNumber;
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
