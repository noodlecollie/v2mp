#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"
#include "V2MPAsm/TempTest.h"
#include "ParseContext.h"
#include "Tokens/TokenMeta.h"

static void ReadAllTokens(V2MPAsm_ParseContext* context)
{
	for ( const char* tokenEnd; !V2MPAsm_ParseContext_InputIsAtEOF(context); V2MPAsm_ParseContext_SeekInput(context, tokenEnd) )
	{
		char tokenBuffer[512];
		const char* tokenBegin;
		V2MPAsm_TokenType tokenType;
		const V2MPAsm_TokenMeta* tokenMeta;
		size_t length;

		tokenBuffer[0] = '\0';
		tokenEnd = NULL;
		tokenBegin = V2MPAsm_ParseContext_GetBeginningOfNextToken(context);

		if ( V2MPAsm_ParseContext_InputIsAtEOF(context) )
		{
			break;
		}

		tokenType = V2MPAsm_TokenMeta_IdentifyToken(tokenBegin, TOKENCTX_DEFAULT);
		tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);

		if ( tokenMeta )
		{
			tokenEnd = V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, tokenBegin, TOKENCTX_DEFAULT);
		}

		if ( !tokenEnd )
		{
			// Default to skipping whitespace.
			tokenEnd = BaseUtil_String_NextWhitespace(tokenBegin);
		}

		length = tokenEnd - tokenBegin;

		if ( length < sizeof(tokenBuffer) )
		{
			memcpy(tokenBuffer, tokenBegin, length);
			tokenBuffer[length] = '\0';
		}
		else
		{
			snprintf(tokenBuffer, sizeof(tokenBuffer), "<Token length %lu exceeded max length of %lu>", length, sizeof(tokenBuffer) - 1);
		}

		printf(
			"%s token of length %lu found at %lu:%lu: %s\n",
			V2MPAsm_TokenMeta_GetTokenTypeString(tokenType),
			length,
			V2MPAsm_ParseContext_GetInputLineNumber(context),
			V2MPAsm_ParseContext_GetInputColumnNumber(context),
			tokenBuffer
		);
	}
}

static void ParseFileData(const char* filePath, const char* buffer, size_t length)
{
	V2MPAsm_ParseContext* context = V2MPAsm_ParseContext_AllocateAndInit();

	if ( !context )
	{
		printf("Failed to create V2MPAsm_ParseContext\n");
		return;
	}

	V2MPAsm_ParseContext_SetInput(context, filePath, buffer, length);

	ReadAllTokens(context);

	V2MPAsm_ParseContext_DeinitAndFree(context);
}

static void ReadFileFromDisk(const char* filePath, FILE* inFile)
{
	size_t length = 0;
	char* buffer;

	fseek(inFile, 0, SEEK_END);
	length = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	printf("File size: %lu bytes\n", length);

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
