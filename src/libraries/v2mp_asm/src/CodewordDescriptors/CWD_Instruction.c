#include "CodewordDescriptors/CWD_Instruction.h"
#include "CodewordDescriptors/CWD_Base.h"
#include "BaseUtil/Heap.h"

static void* Create(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPAsm_CWDInstruction);
}

static void Destroy(void* ptr)
{
	V2MPAsm_CWDInstruction* cwd;

	if ( !ptr )
	{
		return;
	}

	cwd = (V2MPAsm_CWDInstruction*)ptr;

	if ( cwd->argWords )
	{
		BASEUTIL_FREE(cwd->argWords);
	}

	BASEUTIL_FREE(cwd);
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

const V2MPAsm_InstructionMeta* V2MPAsm_CWDInstruction_GetInstructionMeta(const V2MPAsm_CWDInstruction* cwd)
{
	return cwd ? cwd->instructionMeta : NULL;
}

bool V2MPAsm_CWDInstruction_SetInstructionMeta(V2MPAsm_CWDInstruction* cwd, const V2MPAsm_InstructionMeta* meta)
{
	size_t numArgRequired;

	if ( !cwd )
	{
		return false;
	}

	if ( meta == cwd->instructionMeta )
	{
		return true;
	}

	if ( cwd->argWords )
	{
		BASEUTIL_FREE(cwd->argWords);
	}

	cwd->argWords = NULL;
	cwd->numArgWords = 0;
	cwd->instructionMeta = NULL;

	if ( !meta )
	{
		return true;
	}

	numArgRequired = V2MPAsm_InstructionMeta_GetArgCount(meta);

	if ( numArgRequired > 0 )
	{
		cwd->argWords = (V2MPAsm_Word*)BASEUTIL_CALLOC(numArgRequired, sizeof(*cwd->argWords));

		if ( !cwd->argWords )
		{
			return false;
		}
	}

	cwd->numArgWords = numArgRequired;
	cwd->instructionMeta = meta;

	return true;
}

size_t V2MPAsm_CWDInstruction_GetInstructionArgCount(const V2MPAsm_CWDInstruction* cwd)
{
	return cwd ? cwd->numArgWords : 0;
}

V2MPAsm_Word V2MPAsm_CWDInstruction_GetInstructionArg(const V2MPAsm_CWDInstruction* cwd, size_t index)
{
	return (cwd && index < cwd->numArgWords) ? cwd->argWords[index] : 0;
}

void V2MPAsm_CWDInstruction_SetInstructionArg(const V2MPAsm_CWDInstruction* cwd, size_t index, V2MPAsm_Word arg)
{
	if ( !cwd || index >= cwd->numArgWords )
	{
		return;
	}

	cwd->argWords[index] = arg;
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
