#include <stdbool.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"
#include "CodewordDescriptors/CWDList.h"
#include "CodewordDescriptors/CWD_Base.h"

#define LIST_CAPACITY_INCREASE_INCREMENT 10

static inline void FreeCWDs(V2MPAsm_CWDList* list, size_t begin, size_t end)
{
	size_t index;

	for ( index = begin; index < end; ++index )
	{
		V2MPAsm_CWDBase_DeinitAndFree(list->head[index]);
		list->head[index] = NULL;
	}
}

static inline void FreeAllCWDs(V2MPAsm_CWDList* list)
{
	FreeCWDs(list, 0, list->size);
}

static bool ChangeCapacity(V2MPAsm_CWDList* list, size_t newCapacity)
{
	if ( newCapacity == list->capacity )
	{
		return true;
	}

	if ( newCapacity < 1 )
	{
		FreeAllCWDs(list);

		list->head = NULL;
		list->size = 0;
		list->capacity = 0;

		return true;
	}

	if ( list->capacity < 1 )
	{
		list->head = BASEUTIL_MALLOC(newCapacity * sizeof(*list->head));

		if ( !list->head )
		{
			return false;
		}

		list->capacity = newCapacity;

		return true;
	}

	if ( newCapacity > list->capacity )
	{
		V2MPAsm_CWDBase** newHead;

		newHead = BASEUTIL_REALLOC(list->head, newCapacity * sizeof(*list->head));

		if ( !newHead )
		{
			return false;
		}

		list->head = newHead;
		list->capacity = newCapacity;

		return true;
	}

	FreeCWDs(list, BASEUTIL_MIN(newCapacity, list->size), list->size);

	list->size = BASEUTIL_MIN(newCapacity, list->size);
	list->capacity = newCapacity;

	return true;
}

V2MPAsm_CWDList* V2MPAsm_CWDList_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPAsm_CWDList);
}

void V2MPAsm_CWDList_DeinitAndFree(V2MPAsm_CWDList* list)
{
	if ( !list )
	{
		return;
	}

	if ( list->head )
	{
		FreeAllCWDs(list);
		BASEUTIL_FREE(list->head);
	}

	BASEUTIL_FREE(list);
}

size_t V2MPAsm_CWDList_GetSize(const V2MPAsm_CWDList* list)
{
	return list ? list->size : 0;
}

V2MPAsm_CWDBase* V2MPAsm_CWDList_AppendNew(V2MPAsm_CWDList* list, V2MPAsm_CWD_Type type)
{
	V2MPAsm_CWDBase* cwd;

	do
	{
		if ( !list )
		{
			break;
		}

		cwd = V2MPAsm_CWDBase_AllocateAndInit(type);

		if ( !cwd )
		{
			break;
		}

		if ( list->size == list->capacity && !ChangeCapacity(list, list->capacity + LIST_CAPACITY_INCREASE_INCREMENT) )
		{
			break;
		}

		list->head[list->size++] = cwd;

		return cwd;
	}
	while ( false );

	if ( cwd )
	{
		V2MPAsm_CWDBase_DeinitAndFree(cwd);
	}

	return NULL;
}

V2MPAsm_CWDBase* V2MPAsm_CWDList_GetItem(V2MPAsm_CWDList* list, size_t index)
{
	return (list && index <= list->size) ? list->head[index] : NULL;
}
