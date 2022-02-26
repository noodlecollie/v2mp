#include <stdlib.h>
#include <string.h>
#include "Tokens/TokenList.h"
#include "BaseUtil/Heap.h"
#include "BaseUtil/String.h"

static void DestroyTokenEntry(void* payload)
{
	V2MPAsm_TokenListEntry* entry = (V2MPAsm_TokenListEntry*)payload;

	if ( entry->token )
	{
		BASEUTIL_FREE(entry->token);
	}
}

V2MPAsm_TokenList* V2MPAsm_TokenList_AllocateAndInit(void)
{
	V2MPAsm_TokenList* list;

	list = BASEUTIL_CALLOC_STRUCT(V2MPAsm_TokenList);

	if ( !list )
	{
		return NULL;
	}

	list->doubleLL = V2MPSC_DoubleLL_AllocateAndInit(sizeof(V2MPAsm_TokenListEntry), &DestroyTokenEntry);

	if ( !list->doubleLL )
	{
		V2MPAsm_TokenList_DeinitAndFree(list);
		return NULL;
	}

	return list;
}

void V2MPAsm_TokenList_DeinitAndFree(V2MPAsm_TokenList* list)
{
	if ( !list )
	{
		return;
	}

	if ( list->doubleLL )
	{
		V2MPSC_DoubleLL_DeinitAndFree(list->doubleLL);
		list->doubleLL = NULL;
	}

	BASEUTIL_FREE(list);
}

size_t V2MPAsm_TokenList_GetTokenCount(const V2MPAsm_TokenList* list)
{
	return list ? V2MPSC_DoubleLL_GetNodeCount(list->doubleLL) : 0;
}

void V2MPAsm_TokenList_Clear(V2MPAsm_TokenList* list)
{
	if ( !list )
	{
		return;
	}

	V2MPSC_DoubleLL_Clear(list->doubleLL);
}

V2MPAsm_TokenListEntry* V2MPAsm_TokenList_AppendNewEntry(V2MPAsm_TokenList* list, const char* token, size_t length)
{
	V2MPSC_DoubleLL_Node* node;
	V2MPAsm_TokenListEntry* entry;

	if ( !list || !token || length < 1 )
	{
		return NULL;
	}

	node = V2MPSC_DoubleLL_AppendToTail(list->doubleLL);

	if ( !node )
	{
		return NULL;
	}

	entry = (V2MPAsm_TokenListEntry*)V2MPSC_DoubleLLNode_GetPayload(node);

	if ( !entry )
	{
		V2MPSC_DoubleLLNode_Destroy(node);
		return NULL;
	}

	entry->ownerNode = node;
	entry->tokenLength = length;
	entry->token = BASEUTIL_MALLOC(entry->tokenLength + 1);

	if ( !entry->token )
	{
		V2MPSC_DoubleLLNode_Destroy(node);
		return NULL;
	}

	memcpy(entry->token, token, entry->tokenLength);
	entry->token[entry->tokenLength] = '\0';

	return entry;
}

V2MPAsm_TokenListEntry* V2MPAsm_TokenList_GetFirstEntry(const V2MPAsm_TokenList* list)
{
	if ( !list )
	{
		return NULL;
	}

	return (V2MPAsm_TokenListEntry*)V2MPSC_DoubleLLNode_GetPayload(V2MPSC_DoubleLL_GetHead(list->doubleLL));
}

V2MPAsm_TokenListEntry* V2MPAsm_TokenList_GetNextEntry(const V2MPAsm_TokenListEntry* entry)
{
	if ( !entry )
	{
		return NULL;
	}

	return (V2MPAsm_TokenListEntry*)V2MPSC_DoubleLLNode_GetPayload(V2MPSC_DoubleLLNode_GetNext(entry->ownerNode));
}

char* V2MPAsm_TokenListEntry_GetToken(const V2MPAsm_TokenListEntry* entry)
{
	return entry ? entry->token : NULL;
}

size_t V2MPAsm_TokenListEntry_GetTokenLength(const V2MPAsm_TokenListEntry* entry)
{
	return entry ? entry->tokenLength : 0;
}

size_t V2MPAsm_TokenListEntry_GetTokenLine(const V2MPAsm_TokenListEntry* entry)
{
	return entry ? entry->inputLine : 0;
}

void V2MPAsm_TokenListEntry_SetTokenLine(V2MPAsm_TokenListEntry* entry, size_t line)
{
	if ( !entry )
	{
		return;
	}

	entry->inputLine = line;
}

size_t V2MPAsm_TokenListEntry_GetTokenColumn(const V2MPAsm_TokenListEntry* entry)
{
	return entry ? entry->inputColumn : 0;
}

void V2MPAsm_TokenListEntry_SetTokenColumn(V2MPAsm_TokenListEntry* entry, size_t column)
{
	if ( !entry )
	{
		return;
	}

	entry->inputColumn = column;
}
