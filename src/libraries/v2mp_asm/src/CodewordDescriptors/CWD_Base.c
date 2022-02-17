#include <stdlib.h>
#include "CodewordDescriptors/CWD_Base.h"
#include "BaseUtil/Heap.h"

V2MPAsm_CWDBase* V2MPAsm_CWDBase_AllocateAndInit(V2MPAsm_CWD_Type type)
{
	V2MPAsm_CWDBase* cwdBase;
	const V2MP_CWD_Factory* factory;

	factory = V2MPAsm_CWD_GetFactory(type);

	if ( !factory || !factory->create || !factory->destroy )
	{
		return NULL;
	}

	cwdBase = BASEUTIL_CALLOC_STRUCT(V2MPAsm_CWDBase);

	if ( !cwdBase )
	{
		return NULL;
	}

	cwdBase->type = type;
	cwdBase->data = factory->create();

	if ( !cwdBase->data )
	{
		BASEUTIL_FREE(cwdBase);
		return NULL;
	}

	return cwdBase;
}

void V2MPAsm_CWDBase_DeinitAndFree(V2MPAsm_CWDBase* cwdBase)
{
	const V2MP_CWD_Factory* factory;

	if ( !cwdBase )
	{
		return;
	}

	// Should always be valid, since we checked in AllocateAndInit():
	factory = V2MPAsm_CWD_GetFactory(cwdBase->type);
	factory->destroy(cwdBase->data);

	BASEUTIL_FREE(cwdBase);
}

V2MPAsm_CWD_Type V2MPAsm_CWDBase_GetType(const V2MPAsm_CWDBase* cwdBase)
{
	return cwdBase ? cwdBase->type : V2MPASM_CWD_INVALID;
}
