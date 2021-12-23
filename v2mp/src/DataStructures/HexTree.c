#include <string.h>
#include <stdlib.h>
#include "HexTree.h"

#define MAX_DEPTH 3
#define CLAMP_DEPTH(d) ((d) > MAX_DEPTH ? MAX_DEPTH : (d))

static inline V2MP_Word GetKeyFragment(V2MP_Word key, size_t depth)
{
	const size_t shift = 4 * (MAX_DEPTH - CLAMP_DEPTH(depth));
	return (key & (0xF << shift)) >> shift;
}

static void DeallocateNodesRecursive(size_t depth, V2MP_HexTreeNode* node)
{
	if ( !node )
	{
		return;
	}

	// Leaf nodes do not own their contents.
	if ( depth < MAX_DEPTH )
	{
		size_t index;

		for ( index = 0; index < 16; ++index )
		{
			DeallocateNodesRecursive(depth + 1, (V2MP_HexTreeNode*)node->slots[index]);
			node->slots[index] = NULL;
		}
	}

	// Only free the node if it's not the root (we assume the user owns this).
	if ( depth > 0 )
	{
		free(node);
	}
}

static void** GetLeafSlot(V2MP_HexTreeNode* root, V2MP_Word key, bool createIfNonExistent)
{
	size_t depth;
	V2MP_HexTreeNode* node;

	if ( !root )
	{
		return NULL;
	}

	node = root;

	for ( depth = 0; depth <= MAX_DEPTH; ++depth )
	{
		V2MP_Word keyFrag = GetKeyFragment(key, depth);
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

				(*slot) = calloc(1, sizeof(V2MP_HexTreeNode));

				if ( !(*slot) )
				{
					break;
				}
			}

			node = (V2MP_HexTreeNode*)(*slot);
		}
		else
		{
			// We reached the leaf node.
			return slot;
		}
	}

	return NULL;
}

void V2MP_HexTree_InitRoot(V2MP_HexTreeNode* root)
{
	if ( !root )
	{
		return;
	}

	memset(root, 0, sizeof(*root));
}

void V2MP_HexTree_DeinitRoot(V2MP_HexTreeNode* root)
{
	DeallocateNodesRecursive(0, root);
}

bool V2MP_HexTree_Insert(V2MP_HexTreeNode* root, V2MP_Word key, void* value)
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

void* V2MP_HexTree_Remove(V2MP_HexTreeNode* root, V2MP_Word key)
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

void* V2MP_HexTree_Find(V2MP_HexTreeNode* root, V2MP_Word key)
{
	void** slot;

	slot = GetLeafSlot(root, key, false);
	return slot ? *slot : NULL;
}
