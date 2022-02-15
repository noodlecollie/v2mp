#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "V2MPAsm/TempTest.h"
#include "V2MPAsm/Parser.h"

FILE* OpenFile(const char* path, const char* mode)
{
#ifdef _WIN32
	FILE* outFile = NULL;
	fopen_s(&outFile, path, mode);
	return outFile;
#else
	return fopen(path, mode);
#endif
}

static void ParseFileData(const char* filePath, const char* buffer, size_t length)
{
	V2MPAsm_Parser* parser = V2MPAsm_Parser_AllocateAndInit();

	if ( !parser )
	{
		printf("Failed to create V2MPAsm_Parser\n");
		return;
	}

	V2MPAsm_Parser_SetInput(parser, filePath, buffer, length);

	if ( V2MPAsm_Parser_Parse(parser) )
	{
		V2MPAsm_Parser_ExceptionNode* node;

		for ( node = V2MPAsm_Parser_GetFirstExceptionNode(parser); node; node = V2MPAsm_Parser_GetNextExceptionNode(node) )
		{
			V2MPAsm_ParseException* exception;
			char exDesc[512];

			exception = V2MPAsm_Parser_GetExceptionFromNode(node);
			V2MPAsm_ParseException_ToString(exception, exDesc, sizeof(exDesc));

			printf("Parse exception: %s\n", exDesc);

		}
	}
	else
	{
		printf("Failed to run Parse() on file.\n");
	}

	V2MPAsm_Parser_DeinitAndFree(parser);
}

static void ReadFileFromDisk(const char* filePath, FILE* inFile)
{
	size_t length = 0;
	char* buffer;

	fseek(inFile, 0, SEEK_END);
	length = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	printf("File size: %zu bytes\n", length);

	buffer = (char*)BASEUTIL_MALLOC(length);

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

	inFile = OpenFile(filePath, "r");

	if ( !inFile )
	{
		printf("Could not open file %s\n", filePath);
		return;
	}

	printf("Reading file %s\n", filePath);
	ReadFileFromDisk(filePath, inFile);

	fclose(inFile);
}
