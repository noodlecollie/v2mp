#include "CodewordDescriptors/CWD_Label.h"
#include "CodewordDescriptors/CWD_Base.h"
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"

static void* Create(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPAsm_CWDLabel);
}

static void Destroy(void* ptr)
{
	V2MPAsm_CWDLabel* cwd;

	if ( !ptr )
	{
		return;
	}

	cwd = (V2MPAsm_CWDLabel*)ptr;

	if ( cwd->name )
	{
		BASEUTIL_FREE(cwd->name);
	}

	BASEUTIL_FREE(cwd);
}

const V2MP_CWD_Factory V2MP_CWDLabel_Factory =
{
	&Create,
	&Destroy
};

V2MPAsm_CWDLabel* V2MPAsm_CWDLabel_Cast(const struct V2MPAsm_CWDBase* cwdBase)
{
	return (cwdBase && cwdBase->type == V2MPASM_CWD_LABEL)
		? (V2MPAsm_CWDLabel*)cwdBase->data
		: NULL;
}

const char* V2MPAsm_CWDLabel_GetName(const V2MPAsm_CWDLabel* label)
{
	return label ? label->name : NULL;
}

bool V2MPAsm_CWDLabel_SetName(V2MPAsm_CWDLabel* label, const char* name)
{
	if ( !label )
	{
		return false;
	}

	if ( label->name )
	{
		BASEUTIL_FREE(label->name);
		label->name = NULL;
	}

	if ( !name )
	{
		return true;
	}

	label->name = BaseUtil_String_Duplicate(name);

	return label->name != NULL;
}
