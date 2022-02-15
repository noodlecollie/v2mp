#ifndef V2MPASM_COMMANDMETA_H
#define V2MPASM_COMMANDMETA_H

#include <stddef.h>

typedef struct V2MPAsm_CommandArg
{
	const char* name;
	size_t highBitIndex;
	size_t numBits;
} V2MPAsm_CommandArg;

typedef struct V2MPAsm_CommandMeta
{
	const char* name;
	const V2MPAsm_CommandArg* args;
	size_t numArgs;
} V2MPAsm_CommandMeta;

const V2MPAsm_CommandMeta* V2MPAsm_CommandMeta_FindCommand(const char* name);

#endif // V2MPASM_COMMANDMETA_H
