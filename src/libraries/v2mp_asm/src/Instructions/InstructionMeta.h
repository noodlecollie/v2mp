#ifndef V2MPASM_INSTRUCTIONMETA_H
#define V2MPASM_INSTRUCTIONMETA_H

#include <stddef.h>

#define V2MPASM_MAX_COMMAND_NAME_LENGTH 32

typedef struct V2MPAsm_InstructionArg
{
	const char* name;
	size_t highBitIndex;
	size_t numBits;
} V2MPAsm_InstructionArg;

typedef struct V2MPAsm_InstructionMeta
{
	const char* name;
	const V2MPAsm_InstructionArg* args;
	size_t numArgs;
} V2MPAsm_InstructionMeta;

void V2MPAsm_InstructionMeta_InitTable(void);
void V2MPAsm_InstructionMeta_DestroyTable(void);

const V2MPAsm_InstructionMeta* V2MPAsm_InstructionMeta_FindCommand(const char* name, size_t nameLength);

const char* V2MPAsm_InstructionMeta_GetName(const V2MPAsm_InstructionMeta* meta);
size_t V2MPAsm_InstructionMeta_GetArgCount(const V2MPAsm_InstructionMeta* meta);
const V2MPAsm_InstructionArg* V2MPAsm_InstructionMeta_GetArg(const V2MPAsm_InstructionMeta* meta, size_t index);

const char* V2MPasm_InstructionArg_GetName(const V2MPAsm_InstructionArg* arg);
size_t V2MPasm_InstructionArg_GetHighBitIndex(const V2MPAsm_InstructionArg* arg);
size_t V2MPasm_InstructionArg_GetNumBits(const V2MPAsm_InstructionArg* arg);

#endif // V2MPASM_INSTRUCTIONMETA_H
