#ifndef V2MPASM_CWD_INSTRUCTION_H
#define V2MPASM_CWD_INSTRUCTION_H

#include "Instructions/InstructionMeta.h"
#include "Defs.h"

typedef struct V2MP_CWD_Instruction
{
	const V2MPAsm_InstructionMeta* instructionMeta;
} V2MP_CWD_Instruction;

V2MPAsm_Word V2MPAsm_CWD_Instruction_MakeMachineCodeWord(const V2MP_CWD_Instruction* cwdInstruction);

#endif // V2MPASM_CWD_INSTRUCTION_H
