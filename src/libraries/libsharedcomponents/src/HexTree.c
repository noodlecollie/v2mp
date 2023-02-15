#include <string.h>
#include <stdlib.h>
#include "LibSharedComponents/HexTree.h"
#include "LibBaseUtil/Util.h"
#include "LibBaseUtil/Heap.h"

// Each level of the tree has 16 slots, corresponding to the number of permutations in a half-byte.
#define NUM_SLOTS 16

// Each half-byte is a key fragment, so there are two key fragments in a byte.
#define NUM_KEY_FRAG_BITS 4
#define KEY_FRAGS_PER_BYTE 2
#define KEY_FRAG_MASK 0xF

// The depth of the tree is KEY_FRAGS_PER_BYTE * number of bytes in the key.
// Because the depth begins at 0, MAX_DEPTH is this value - 1.
// Using a uint16_t, the depth of the tree is 4 (so MAX_DEPTH is 3).
#define MAX_DEPTH ((sizeof(uint16_t) * KEY_FRAGS_PER_BYTE) - 1)

#define CLAMP_DEPTH(d) ((d) > MAX_DEPTH ? MAX_DEPTH : (d))

struct V2MPSC_HexTreeNode
{
	void* slots[NUM_SLOTS];
};

static inline uint16_t GetKeyFragment(uint16_t key, size_t depth)
{
	// The deeper in the tree we go, the less significant the group
	// of bits we're interested in.
	const size_t shift = NUM_KEY_FRAG_BITS * (MAX_DEPTH - CLAMP_DEPTH(depth));

	// Mask off these bits and return them as the least significant bits.
	return (key & (KEY_FRAG_MASK << shift)) >> shift;
}

static void DeallocateNodesRecursive(size_t depth, V2MPSC_HexTreeNode* node)
{
	if ( !node )
	{
		return;
	}

	// Leaf nodes do not own their contents.
	if ( depth < MAX_DEPTH )
	{
		size_t index;

		for ( index = 0; index < NUM_SLOTS; ++index )
		{
			DeallocateNodesRecursive(depth + 1, (V2MPSC_HexTreeNode*)node->slots[index]);
			node->slots[index] = NULL;
		}
	}

	// Only free the node if it's not the root (we assume the user owns this).
	if ( depth > 0 )
	{
		free(node);
	}
}

static void** GetLeafSlot(V2MPSC_HexTreeNode* root, uint16_t key, bool createIfNonExistent)
{
	size_t depth;
	V2MPSC_HexTreeNode* node;

	if ( !root )
	{
		return NULL;
	}

	node = root;

	for ( depth = 0; depth <= MAX_DEPTH; ++depth )
	{
		uint16_t keyFrag = GetKeyFragment(key, depth);
		void** slot = &node->slots[keyFrag];

		if ( depth < MAX_DEPTH )
		{
			// Not at leaf yet.

			if ( !(*slot) )
			{
				if ( !createIfNonExistent )
				{
					break;
				}

				(*slot) = BASEUTIL_CALLOC_STRUCT(V2MPSC_HexTreeNode);

				if ( !(*slot) )
				{
					break;
				}
			}

			node = (V2MPSC_HexTreeNode*)(*slot);
		}
		else
		{
			// We reached the leaf node.
			return slot;
		}
	}

	return NULL;
}

V2MPSC_HexTreeNode* V2MPSC_HexTree_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MPSC_HexTreeNode);
}

void V2MPSC_HexTree_DeinitAndFree(V2MPSC_HexTreeNode* root)
{
	DeallocateNodesRecursive(0, root);
}

bool V2MPSC_HexTree_Insert(V2MPSC_HexTreeNode* root, uint16_t key, void* value)
{
	void** slot;

	if ( !value )
	{
		return false;
	}

	slot = GetLeafSlot(root, key, true);

	if ( !slot )
	{
		// Something went wrong while trying to allocate the slot.
		return false;
	}

	if ( *slot )
	{
		// Something was already present in the slot.
		return false;
	}

	*slot = value;
	return true;
}

void* V2MPSC_HexTree_Remove(V2MPSC_HexTreeNode* root, uint16_t key)
{
	void** slot;
	void* item = NULL;

	slot = GetLeafSlot(root, key, false);

	if ( slot )
	{
		item = *slot;
		*slot = NULL;
	}

	return item;
}

void* V2MPSC_HexTree_Find(V2MPSC_HexTreeNode* root, uint16_t key)
{
	void** slot;

	slot = GetLeafSlot(root, key, false);
	return slot ? *slot : NULL;
}
