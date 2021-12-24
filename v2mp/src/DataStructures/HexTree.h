#ifndef V2MP_HEXTREE_H
#define V2MP_HEXTREE_H

#include <stdbool.h>
#include "V2MP/Defs.h"

typedef struct V2MP_HexTreeNode V2MP_HexTreeNode;

size_t V2MP_HexTree_Footprint(void);
V2MP_HexTreeNode* V2MP_HexTree_AllocateAndInit(void);
void V2MP_HexTree_DeinitAndFree(V2MP_HexTreeNode* root);

// Returns true if the insert took place, and false otherwise.
// The insert will not take place if the value is NULL, or if
// there is already something in the tree with the given key.
bool V2MP_HexTree_Insert(V2MP_HexTreeNode* root, V2MP_Word key, void* value);

// Returns the item that was removed, or NULL if there was no item at this key.
// This does not currently free up data structure memory if it leaves a node empty
// (perhaps this is something we want to add in future).
void* V2MP_HexTree_Remove(V2MP_HexTreeNode* root, V2MP_Word key);

void* V2MP_HexTree_Find(V2MP_HexTreeNode* root, V2MP_Word key);

#endif // V2MP_HEXTREE_H
