#ifndef V2MPASM_CWD_BASE_H
#define V2MPASM_CWD_BASE_H

#include "CodewordDescriptors/CWD_Factory.h"

typedef struct V2MPAsm_CWDBase
{
	V2MPAsm_CWD_Type type;
	void* data;
} V2MPAsm_CWDBase;

V2MPAsm_CWDBase* V2MPAsm_CWDBase_AllocateAndInit(V2MPAsm_CWD_Type type);
void V2MPAsm_CWDBase_DeinitAndFree(V2MPAsm_CWDBase* cwdBase);

V2MPAsm_CWD_Type V2MPAsm_CWDBase_GetType(const V2MPAsm_CWDBase* cwdBase);

#endif // V2MPASM_CWD_BASE_H
