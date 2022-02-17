#ifndef V2MPASM_CWD_INSTRUCTION_H
#define V2MPASM_CWD_INSTRUCTION_H

#include "CodewordDescriptors/CWD_Factory.h"
#include "Instructions/InstructionMeta.h"
#include "Defs.h"

struct V2MPAsm_CWDBase;

typedef struct V2MPAsm_CWDInstruction
{
	const V2MPAsm_InstructionMeta* instructionMeta;
} V2MPAsm_CWDInstruction;

extern const V2MP_CWD_Factory V2MP_CWDInstruction_Factory;

V2MPAsm_CWDInstruction* V2MPAsm_CWDInstruction_Cast(const struct V2MPAsm_CWDBase* cwdBase);
V2MPAsm_Word V2MPAsm_CWDInstruction_MakeMachineCodeWord(const V2MPAsm_CWDInstruction* cwdInstruction);

#endif // V2MPASM_CWD_INSTRUCTION_H
