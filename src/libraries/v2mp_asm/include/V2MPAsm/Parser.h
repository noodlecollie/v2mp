#ifndef V2MPASM_PARSER_H
#define V2MPASM_PARSER_H

#include <stdbool.h>
#include "V2MPAsm/LibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPAsm_Parser V2MPAsm_Parser;

API_V2MPASM V2MPAsm_Parser* V2MPAsm_Parser_AllocateAndInit(void);
API_V2MPASM void V2MPAsm_Parser_DeinitAndFree(V2MPAsm_Parser* parser);

API_V2MPASM void V2MPAsm_Parser_SetInput(V2MPAsm_Parser* parser, const char* filePath, const char* data, size_t dataLength);
API_V2MPASM bool V2MPAsm_Parser_HasInput(const V2MPAsm_Parser* parser);

API_V2MPASM bool V2MPAsm_Parser_Parse(V2MPAsm_Parser* parser);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_PARSER_H
