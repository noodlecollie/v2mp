#ifndef V2MPASM_PARSECONTEXT_H
#define V2MPASM_PARSECONTEXT_H

#include <stdbool.h>
#include "V2MPAsm/ParseException.h"
#include "BaseUtil/Filesystem.h"
#include "InputFile.h"

typedef enum V2MPAsm_ParseState
{
	PARSESTATE_DEFAULT = 0,
	PARSESTATE_TERMINATED
} V2MPAsm_ParseState;

typedef struct V2MPAsm_ParseContext
{
	V2MPAsm_InputFile* inputFile;
	char* filePath;
	const char* fileName;

	V2MPAsm_ParseState state;
} V2MPAsm_ParseContext;

V2MPAsm_ParseContext* V2MPAsm_ParseContext_AllocateAndInit(void);
void V2MPAsm_ParseContext_DeinitAndFree(V2MPAsm_ParseContext* context);

V2MPAsm_InputFile* V2MPAsm_ParseContext_GetInputFile(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetFilePath(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetFileName(const V2MPAsm_ParseContext* context);

bool V2MPAsm_ParseContext_SetInput(V2MPAsm_ParseContext* context, const char* filePath, const char* data, size_t length);
bool V2MPAsm_ParseContext_HasInput(const V2MPAsm_ParseContext* context);
bool V2MPAsm_ParseContext_InputIsAtEOF(const V2MPAsm_ParseContext* context);
size_t V2MPAsm_ParseContext_GetInputLineNumber(const V2MPAsm_ParseContext* context);
size_t V2MPAsm_ParseContext_GetInputColumnNumber(const V2MPAsm_ParseContext* context);
const char* V2MPAsm_ParseContext_GetInputCursor(const V2MPAsm_ParseContext* context);

void V2MPAsm_ParseContext_SeekInput(V2MPAsm_ParseContext* context, const char* pos);
const char* V2MPAsm_ParseContext_GetBeginningOfNextToken(V2MPAsm_ParseContext* context);

V2MPAsm_ParseState V2MPAsm_ParseContext_GetParseState(const V2MPAsm_ParseContext* context);
void V2MPAsm_ParseContext_SetParseState(V2MPAsm_ParseContext* context, V2MPAsm_ParseState state);

#endif // V2MPASM_PARSECONTEXT_H
