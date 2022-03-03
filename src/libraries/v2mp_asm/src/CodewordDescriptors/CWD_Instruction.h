#ifndef V2MPASM_CWD_INSTRUCTION_H
#define V2MPASM_CWD_INSTRUCTION_H

#include <stdbool.h>
#include "CodewordDescriptors/CWD_Factory.h"
#include "Instructions/InstructionMeta.h"
#include "Defs.h"
#include "Tokens/TokenMeta_LabelReference.h"

struct V2MPAsm_CWDBase;

typedef struct V2MPAsm_CWDInstruction_LabelRef
{
	char* labelName;
	V2MPAsm_LabelReferenceType refType;
} V2MPAsm_CWDInstruction_LabelRef;

typedef struct V2MPAsm_CWDInstruction_Arg
{
	bool isLabelRef;

	union
	{
		V2MPAsm_CWDInstruction_LabelRef labelRef;
		V2MPAsm_Word numericValue;
	} value;
} V2MPAsm_CWDInstruction_Arg;

typedef struct V2MPAsm_CWDInstruction
{
	const V2MPAsm_InstructionMeta* instructionMeta;
	V2MPAsm_CWDInstruction_Arg* args;
	size_t numArgs;
} V2MPAsm_CWDInstruction;

extern const V2MP_CWD_Factory V2MP_CWDInstruction_Factory;

V2MPAsm_CWDInstruction* V2MPAsm_CWDInstruction_Cast(struct V2MPAsm_CWDBase* cwdBase);

const V2MPAsm_InstructionMeta* V2MPAsm_CWDInstruction_GetInstructionMeta(const V2MPAsm_CWDInstruction* cwd);
bool V2MPAsm_CWDInstruction_SetInstructionMeta(V2MPAsm_CWDInstruction* cwd, const V2MPAsm_InstructionMeta* meta);

size_t V2MPAsm_CWDInstruction_GetInstructionArgCount(const V2MPAsm_CWDInstruction* cwd);
V2MPAsm_CWDInstruction_Arg* V2MPAsm_CWDInstruction_GetInstructionArg(const V2MPAsm_CWDInstruction* cwd, size_t index);

bool V2MPAsm_CWDInstructionArg_SetNumericValue(V2MPAsm_CWDInstruction_Arg* arg, V2MPAsm_Word value);

bool V2MPAsm_CWDInstructionArg_SetLabelRef(
	V2MPAsm_CWDInstruction_Arg* arg,
	const char* labelName,
	V2MPAsm_LabelReferenceType refType
);

bool V2MPAsm_CWDInstruction_MakeMachineCodeWord(const V2MPAsm_CWDInstruction* cwdInstruction, V2MPAsm_Word* outWord);

#endif // V2MPASM_CWD_INSTRUCTION_H
