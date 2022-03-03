#ifndef V2MPASM_CWDLIST_H
#define V2MPASM_CWDLIST_H

#include <stddef.h>
#include "CodewordDescriptors/CWD_Base.h"

typedef struct V2MPAsm_CWDList
{
	V2MPAsm_CWDBase** head;
	size_t size;
	size_t capacity;
} V2MPAsm_CWDList;

V2MPAsm_CWDList* V2MPAsm_CWDList_AllocateAndInit(void);
void V2MPAsm_CWDList_DeinitAndFree(V2MPAsm_CWDList* list);

size_t V2MPAsm_CWDList_GetSize(const V2MPAsm_CWDList* list);
V2MPAsm_CWDBase* V2MPAsm_CWDList_AppendNew(V2MPAsm_CWDList* list, V2MPAsm_CWD_Type type);
V2MPAsm_CWDBase* V2MPAsm_CWDList_GetItem(V2MPAsm_CWDList* list, size_t index);

#endif // V2MPASM_CWDLIST_H
