#ifndef V2MPASM_TOKENLIST_H
#define V2MPASM_TOKENLIST_H

#include "SharedComponents/DoubleLinkedList.h"

typedef struct V2MPAsm_TokenListEntry
{
	V2MPSC_DoubleLL_Node* ownerNode;
	char* token;
	size_t tokenLength;
	size_t inputLine;
	size_t inputColumn;
} V2MPAsm_TokenListEntry;

typedef struct V2MPAsm_TokenList
{
	V2MPSC_DoubleLL* doubleLL;
} V2MPAsm_TokenList;

V2MPAsm_TokenList* V2MPAsm_TokenList_AllocateAndInit(void);
void V2MPAsm_TokenList_DeinitAndFree(V2MPAsm_TokenList* list);
size_t V2MPAsm_TokenList_GetTokenCount(const V2MPAsm_TokenList* list);

void V2MPAsm_TokenList_Clear(V2MPAsm_TokenList* list);
V2MPAsm_TokenListEntry* V2MPAsm_TokenList_AppendNewEntry(V2MPAsm_TokenList* list, const char* token, size_t length);

V2MPAsm_TokenListEntry* V2MPAsm_TokenList_GetFirstEntry(const V2MPAsm_TokenList* list);
V2MPAsm_TokenListEntry* V2MPAsm_TokenList_GetNextEntry(const V2MPAsm_TokenListEntry* entry);

char* V2MPAsm_TokenListEntry_GetToken(const V2MPAsm_TokenListEntry* entry);
size_t V2MPAsm_TokenListEntry_GetTokenLength(const V2MPAsm_TokenListEntry* entry);

size_t V2MPAsm_TokenListEntry_GetTokenLine(const V2MPAsm_TokenListEntry* entry);
void V2MPAsm_TokenListEntry_SetTokenLine(V2MPAsm_TokenListEntry* entry, size_t line);
size_t V2MPAsm_TokenListEntry_GetTokenColumn(const V2MPAsm_TokenListEntry* entry);
void V2MPAsm_TokenListEntry_SetTokenColumn(V2MPAsm_TokenListEntry* entry, size_t column);

#endif // V2MPASM_TOKENLIST_H
