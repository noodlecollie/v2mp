#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Components/DoubleLinkedList.h"
#include "V2MPInternal/Components/HexTree.h"
#include "V2MPInternal/Util/Heap.h"
#include "Modules/DevicePort_Internal.h"

typedef struct DevicePortEntry
{
	V2MP_DevicePort* port;
} DevicePortEntry;

struct V2MP_DevicePortCollection
{
	V2MP_DoubleLL* portList;
	V2MP_HexTreeNode* portTree;
};

static void DeinitDevicePortEntry(void* ptr)
{
	DevicePortEntry* entry = (DevicePortEntry*)ptr;
	V2MP_DevicePort_DeinitAndFree(entry->port);
}

V2MP_DevicePortCollection* V2MP_DevicePortCollection_AllocateAndInit(void)
{
	V2MP_DevicePortCollection* dpc = V2MP_CALLOC_STRUCT(V2MP_DevicePortCollection);

	if ( !dpc )
	{
		return NULL;
	}

	dpc->portList = V2MP_DoubleLL_AllocateAndInit(sizeof(DevicePortEntry), &DeinitDevicePortEntry);
	dpc->portTree = V2MP_HexTree_AllocateAndInit();

	if ( !dpc->portList || !dpc->portTree )
	{
		V2MP_DevicePortCollection_DeinitAndFree(dpc);
		dpc = NULL;
	}

	return dpc;
}

void V2MP_DevicePortCollection_DeinitAndFree(V2MP_DevicePortCollection* dpc)
{
	if ( !dpc )
	{
		return;
	}

	V2MP_HexTree_DeinitAndFree(dpc->portTree);
	V2MP_DoubleLL_DeinitAndFree(dpc->portList);

	V2MP_FREE(dpc);
}

struct V2MP_DevicePort* V2MP_DevicePortCollection_CreatePort(V2MP_DevicePortCollection* dpc, V2MP_Word address)
{
	V2MP_DoubleLL_Node* node = NULL;

	if ( !dpc )
	{
		return NULL;
	}

	do
	{
		DevicePortEntry* entry = NULL;

		node = V2MP_DoubleLL_AppendToTail(dpc->portList);

		if ( !node )
		{
			break;
		}

		entry = (DevicePortEntry*)V2MP_DoubleLLNode_GetPayload(node);
		entry->port = V2MP_DevicePort_AllocateAndInit();

		if ( !entry->port )
		{
			break;
		}

		if ( !V2MP_HexTree_Insert(dpc->portTree, address, node) )
		{
			break;
		}

		V2MP_DevicePort_SetAddress(entry->port, address);

		return entry->port;
	}
	while ( false );

	if ( node )
	{
		V2MP_DoubleLLNode_Destroy(node);
	}

	return NULL;
}

bool V2MP_DevicePortCollection_DestroyPort(V2MP_DevicePortCollection* dpc, V2MP_Word address)
{
	V2MP_DoubleLL_Node* node = NULL;
	DevicePortEntry* entry = NULL;

	if ( !dpc )
	{
		return false;
	}

	node = (V2MP_DoubleLL_Node*)V2MP_HexTree_Remove(dpc->portTree, address);

	if ( !node )
	{
		return false;
	}

	entry = (DevicePortEntry*)V2MP_DoubleLLNode_GetPayload(node);

	V2MP_DevicePort_DeinitAndFree(entry->port);
	V2MP_DoubleLLNode_Destroy(node);

	return true;
}

struct V2MP_DevicePort* V2MP_DevicePortCollection_GetPort(V2MP_DevicePortCollection* dpc, V2MP_Word address)
{
	V2MP_DoubleLL_Node* node = NULL;
	DevicePortEntry* entry = NULL;

	if ( !dpc )
	{
		return NULL;
	}

	node = (V2MP_DoubleLL_Node*)V2MP_HexTree_Find(dpc->portTree, address);

	if ( !node )
	{
		return NULL;
	}

	entry = (DevicePortEntry*)V2MP_DoubleLLNode_GetPayload(node);

	return entry->port;
}
