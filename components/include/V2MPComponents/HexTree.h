#ifndef V2MPCOMPONENTS_HEXTREE_H
#define V2MPCOMPONENTS_HEXTREE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPCpts_HexTreeNode V2MPCpts_HexTreeNode;

size_t V2MPCpts_HexTree_Footprint(void);
V2MPCpts_HexTreeNode* V2MPCpts_HexTree_AllocateAndInit(void);
void V2MPCpts_HexTree_DeinitAndFree(V2MPCpts_HexTreeNode* root);

// Returns true if the insert took place, and false otherwise.
// The insert will not take place if the value is NULL, or if
// there is already something in the tree with the given key.
bool V2MPCpts_HexTree_Insert(V2MPCpts_HexTreeNode* root, uint16_t key, void* value);

// Returns the item that was removed, or NULL if there was no item at this key.
// This does not currently free up data structure memory if it leaves a node empty
// (perhaps this is something we want to add in future).
void* V2MPCpts_HexTree_Remove(V2MPCpts_HexTreeNode* root, uint16_t key);

void* V2MPCpts_HexTree_Find(V2MPCpts_HexTreeNode* root, uint16_t key);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPCOMPONENTS_HEXTREE_H
