#include "CodewordDescriptors/CWD_Instruction.h"
#include "CodewordDescriptors/CWD_Base.h"
#include "BaseUtil/Heap.h"

static void* Create(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPAsm_CWDInstruction);
}

static void Destroy(void* ptr)
{
	if ( ptr )
	{
		BASEUTIL_FREE(ptr);
	}
}

const V2MP_CWD_Factory V2MP_CWDInstruction_Factory =
{
	&Create,
	&Destroy
};

V2MPAsm_CWDInstruction* V2MPAsm_CWDInstruction_Cast(const struct V2MPAsm_CWDBase* cwdBase)
{
	return (cwdBase && cwdBase->type == V2MPASM_CWD_INSTRUCTION)
		? (V2MPAsm_CWDInstruction*)cwdBase->data
		: NULL;
}

void V2MPAsm_CWDInstruction_SetInstructionMeta(V2MPAsm_CWDInstruction* cwd, const V2MPAsm_InstructionMeta* meta)
{
	if ( !cwd )
	{
		return;
	}

	cwd->instructionMeta = meta;
}

bool V2MPAsm_CWDInstruction_MakeMachineCodeWord(const V2MPAsm_CWDInstruction* cwdInstruction, V2MPAsm_Word* outWord)
{
	if ( !cwdInstruction || !outWord )
	{
		return false;
	}

	// TODO
	return false;
}
