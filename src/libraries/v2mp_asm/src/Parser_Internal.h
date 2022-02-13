#ifndef V2MPASM_PARSER_INTERNAL_H
#define V2MPASM_PARSER_INTERNAL_H

#include "ParseContext.h"

struct V2MPAsm_Parser
{
	V2MPAsm_ParseContext* context;
};

// This struct is (in a slightly hacky way) used as an opaque
// pointer for a ParseContext exception node in the library interface.
// It just exists for type safety (hence why we don't use a void*).
// Wherever a V2MPAsm_Parser_ExceptionNode* is passed, underneat it's
// actually just a V2MPAsm_ParseContext_ExceptionNode*.
struct V2MPAsm_Parser_ExceptionNode
{
	int dummy;
};

#endif // V2MPASM_PARSER_INTERNAL_H
