#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "SharedComponents/DoubleLinkedList.h"
#include "SharedComponents/HexTree.h"
#include "BaseUtil/Heap.h"
#include "Modules/DevicePort_Internal.h"

typedef struct DevicePortEntry
{
	V2MP_DevicePort* port;
} DevicePortEntry;

struct V2MP_DevicePortCollection
{
	V2MPSC_DoubleLL* portList;
	V2MPSC_HexTreeNode* portTree;
};

static void DeinitDevicePortEntry(void* ptr)
{
	DevicePortEntry* entry = (DevicePortEntry*)ptr;
	V2MP_DevicePort_DeinitAndFree(entry->port);
}

V2MP_DevicePortCollection* V2MP_DevicePortCollection_AllocateAndInit(void)
{
	V2MP_DevicePortCollection* dpc = BASEUTIL_CALLOC_STRUCT(V2MP_DevicePortCollection);

	if ( !dpc )
	{
		return NULL;
	}

	dpc->portList = V2MPSC_DoubleLL_AllocateAndInit(sizeof(DevicePortEntry), &DeinitDevicePortEntry);
	dpc->portTree = V2MPSC_HexTree_AllocateAndInit();

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

	V2MPSC_HexTree_DeinitAndFree(dpc->portTree);
	V2MPSC_DoubleLL_DeinitAndFree(dpc->portList);

	BASEUTIL_FREE(dpc);
}

struct V2MP_DevicePort* V2MP_DevicePortCollection_CreatePort(V2MP_DevicePortCollection* dpc, V2MP_Word address)
{
	V2MPSC_DoubleLL_Node* node = NULL;

	if ( !dpc )
	{
		return NULL;
	}

	do
	{
		DevicePortEntry* entry = NULL;

		node = V2MPSC_DoubleLL_AppendToTail(dpc->portList);

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

		if ( !V2MPSC_HexTree_Insert(dpc->portTree, address, node) )
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
	V2MPSC_DoubleLL_Node* node = NULL;

	if ( !dpc )
	{
		return false;
	}

	node = (V2MPSC_DoubleLL_Node*)V2MPSC_HexTree_Remove(dpc->portTree, address);

	if ( !node )
	{
		return false;
	}

	// The contents of the node are cleaned up when the node is destroyed.
	V2MP_DoubleLLNode_Destroy(node);

	return true;
}

struct V2MP_DevicePort* V2MP_DevicePortCollection_GetPort(V2MP_DevicePortCollection* dpc, V2MP_Word address)
{
	V2MPSC_DoubleLL_Node* node = NULL;
	DevicePortEntry* entry = NULL;

	if ( !dpc )
	{
		return NULL;
	}

	node = (V2MPSC_DoubleLL_Node*)V2MPSC_HexTree_Find(dpc->portTree, address);

	if ( !node )
	{
		return NULL;
	}

	entry = (DevicePortEntry*)V2MP_DoubleLLNode_GetPayload(node);

	return entry->port;
}
