#ifndef V2MPASM_PARSECONTEXT_H
#define V2MPASM_PARSECONTEXT_H

#include <stdbool.h>
#include "InputFile.h"
#include "BaseUtil/Filesystem.h"

typedef struct V2MPAsm_ParseContext
{
	V2MPAsm_InputFile* inputFile;

	char* filePath;
	const char* fileName;

	char* lineBuffer;
	size_t lineBufferSize;
} V2MPAsm_ParseContext;

V2MPAsm_ParseContext* V2MPAsm_ParseContext_AllocateAndInit(void);
void V2MPAsm_ParseContext_DeinitAndFree(V2MPAsm_ParseContext* context);

V2MPAsm_InputFile* V2MPAsm_ParseContext_GetInputFile(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetFilePath(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetFileName(const V2MPAsm_ParseContext* context);

bool V2MPAsm_ParseContext_SetInput(V2MPAsm_ParseContext* context, const char* filePath, const V2MPAsm_Byte* data, size_t length);
bool V2MPAsm_ParseContext_HasInput(const V2MPAsm_ParseContext* context);

bool V2MPAsm_ParseContext_AllocateLineBuffer(V2MPAsm_ParseContext* context, size_t bufferSize);
char* V2MPAsm_ParseContext_GetLineBuffer(const V2MPAsm_ParseContext* context);
size_t V2MPAsm_ParseContext_GetLineBufferSize(const V2MPAsm_ParseContext* context);

#endif // V2MPASM_PARSECONTEXT_H
