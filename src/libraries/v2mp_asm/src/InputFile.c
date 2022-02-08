#include "V2MPAsm/InputFile.h"
#include "BaseUtil/Heap.h"
#include "InputFile_Internal.h"

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
