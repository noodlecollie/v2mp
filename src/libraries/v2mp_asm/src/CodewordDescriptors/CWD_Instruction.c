#include "CodewordDescriptors/CWD_Instruction.h"
#include "CodewordDescriptors/CWD_Base.h"
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"

static inline void FreeArg(V2MPAsm_CWDInstruction_Arg* arg)
{
	if ( arg->isLabelRef && arg->value.labelRef.labelName )
	{
		BASEUTIL_FREE(arg->value.labelRef.labelName);
	}
}

static inline void FreeAllArgs(V2MPAsm_CWDInstruction* cwd)
{
	size_t index;

	if ( cwd->args )
	{
		for ( index = 0; index < cwd->numArgs; ++index )
		{
			FreeArg(&cwd->args[index]);
		}
	}

	BASEUTIL_FREE(cwd->args);
}

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

	FreeAllArgs(cwd);

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
	size_t numArgsRequired;

	if ( !cwd )
	{
		return false;
	}

	if ( meta == cwd->instructionMeta )
	{
		return true;
	}

	FreeAllArgs(cwd);

	cwd->args = NULL;
	cwd->numArgs = 0;
	cwd->instructionMeta = NULL;

	if ( !meta )
	{
		return true;
	}

	numArgsRequired = V2MPAsm_InstructionMeta_GetArgCount(meta);

	if ( numArgsRequired > 0 )
	{
		cwd->args = (V2MPAsm_CWDInstruction_Arg*)BASEUTIL_CALLOC(numArgsRequired, sizeof(*cwd->args));

		if ( !cwd->args )
		{
			return false;
		}
	}

	cwd->numArgs = numArgsRequired;
	cwd->instructionMeta = meta;

	return true;
}

size_t V2MPAsm_CWDInstruction_GetInstructionArgCount(const V2MPAsm_CWDInstruction* cwd)
{
	return cwd ? cwd->numArgs : 0;
}

V2MPAsm_CWDInstruction_Arg* V2MPAsm_CWDInstruction_GetInstructionArg(const V2MPAsm_CWDInstruction* cwd, size_t index)
{
	return (cwd && index < cwd->numArgs) ? &cwd->args[index] : 0;
}

bool V2MPAsm_CWDInstructionArg_SetNumericValue(V2MPAsm_CWDInstruction_Arg* arg, V2MPAsm_Word value)
{
	if ( !arg )
	{
		return false;
	}

	FreeArg(arg);

	arg->isLabelRef = false;
	arg->value.numericValue = value;

	return true;
}

bool V2MPAsm_CWDInstructionArg_SetLabelRef(
	V2MPAsm_CWDInstruction_Arg* arg,
	const char* labelName,
	V2MPAsm_LabelReferenceType refType
)
{
	char* labelNameDuplicate;

	if ( !arg || !labelName )
	{
		return false;
	}

	labelNameDuplicate = BaseUtil_String_Duplicate(labelName);

	if ( !labelNameDuplicate )
	{
		return false;
	}

	FreeArg(arg);

	arg->isLabelRef = true;
	arg->value.labelRef.labelName = labelNameDuplicate;
	arg->value.labelRef.refType = refType;

	return true;
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
