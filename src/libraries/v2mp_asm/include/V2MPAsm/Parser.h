#ifndef V2MPASM_PARSER_H
#define V2MPASM_PARSER_H

#include <stdbool.h>
#include "V2MPAsm/LibExport.h"
#include "V2MPAsm/ParseException.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPAsm_Parser V2MPAsm_Parser;
typedef struct V2MPAsm_Parser_ExceptionNode V2MPAsm_Parser_ExceptionNode;

API_V2MPASM V2MPAsm_Parser* V2MPAsm_Parser_AllocateAndInit(void);
API_V2MPASM void V2MPAsm_Parser_DeinitAndFree(V2MPAsm_Parser* parser);

API_V2MPASM void V2MPAsm_Parser_SetInput(V2MPAsm_Parser* parser, const char* filePath, const char* data, size_t dataLength);
API_V2MPASM bool V2MPAsm_Parser_HasInput(const V2MPAsm_Parser* parser);

API_V2MPASM bool V2MPAsm_Parser_Parse(V2MPAsm_Parser* parser);

API_V2MPASM size_t V2MPAsm_Parser_GetExceptionCount(const V2MPAsm_Parser* parser);
API_V2MPASM V2MPAsm_Parser_ExceptionNode* V2MPAsm_Parser_GetFirstExceptionNode(const V2MPAsm_Parser* parser);
API_V2MPASM V2MPAsm_Parser_ExceptionNode* V2MPAsm_Parser_GetNextExceptionNode(V2MPAsm_Parser_ExceptionNode* node);
API_V2MPASM V2MPAsm_ParseException* V2MPAsm_Parser_GetExceptionFromNode(V2MPAsm_Parser_ExceptionNode* node);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_PARSER_H
