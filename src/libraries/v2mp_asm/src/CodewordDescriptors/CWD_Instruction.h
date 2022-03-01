#ifndef V2MPASM_CWD_INSTRUCTION_H
#define V2MPASM_CWD_INSTRUCTION_H

#include <stdbool.h>
#include "CodewordDescriptors/CWD_Factory.h"
#include "Instructions/InstructionMeta.h"
#include "Defs.h"

struct V2MPAsm_CWDBase;

typedef struct V2MPAsm_CWDInstruction
{
	const V2MPAsm_InstructionMeta* instructionMeta;
	V2MPAsm_Word* argWords;
	size_t numArgWords;
} V2MPAsm_CWDInstruction;

extern const V2MP_CWD_Factory V2MP_CWDInstruction_Factory;

V2MPAsm_CWDInstruction* V2MPAsm_CWDInstruction_Cast(const struct V2MPAsm_CWDBase* cwdBase);

const V2MPAsm_InstructionMeta* V2MPAsm_CWDInstruction_GetInstructionMeta(const V2MPAsm_CWDInstruction* cwd);
bool V2MPAsm_CWDInstruction_SetInstructionMeta(V2MPAsm_CWDInstruction* cwd, const V2MPAsm_InstructionMeta* meta);

size_t V2MPAsm_CWDInstruction_GetInstructionArgCount(const V2MPAsm_CWDInstruction* cwd);
V2MPAsm_Word V2MPAsm_CWDInstruction_GetInstructionArg(const V2MPAsm_CWDInstruction* cwd, size_t index);
void V2MPAsm_CWDInstruction_SetInstructionArg(const V2MPAsm_CWDInstruction* cwd, size_t index, V2MPAsm_Word arg);

bool V2MPAsm_CWDInstruction_MakeMachineCodeWord(const V2MPAsm_CWDInstruction* cwdInstruction, V2MPAsm_Word* outWord);

#endif // V2MPASM_CWD_INSTRUCTION_H
