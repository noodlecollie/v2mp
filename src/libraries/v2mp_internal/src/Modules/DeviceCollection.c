#include "V2MPInternal/Modules/DeviceCollection.h"
#include "V2MPInternal/Modules/Device.h"
#include "SharedComponents/DoubleLinkedList.h"
#include "BaseUtil/Heap.h"
#include "Modules/Device_Internal.h"

typedef struct DeviceEntry
{
	V2MP_Device* device;
} DeviceEntry;

struct V2MP_DeviceCollection
{
	V2MPSC_DoubleLL* deviceList;
};

static void DeinitDeviceEntry(void* ptr)
{
	DeviceEntry* entry = (DeviceEntry*)ptr;
	V2MP_Device_DeinitAndFree(entry->device);
}

static V2MP_Device* GetNextDevice(const V2MP_DeviceCollection* dc, V2MP_Device* device)
{
	V2MPSC_DoubleLL_Node* node;
	DeviceEntry* entry;

	if ( !dc )
	{
		return NULL;
	}

	node = device ? V2MP_DoubleLLNode_GetNext(device->ownerNode) : V2MPSC_DoubleLL_GetHead(dc->deviceList);

	if ( !node )
	{
		return NULL;
	}

	entry = (DeviceEntry*)V2MP_DoubleLLNode_GetPayload(node);

	if ( !entry )
	{
		return NULL;
	}

	return entry->device;
}

V2MP_DeviceCollection* V2MP_DeviceCollection_AllocateAndInit(void)
{
	V2MP_DeviceCollection* dc = BASEUTIL_CALLOC_STRUCT(V2MP_DeviceCollection);

	if ( !dc )
	{
		return NULL;
	}

	dc->deviceList = V2MPSC_DoubleLL_AllocateAndInit(sizeof(DeviceEntry), &DeinitDeviceEntry);

	if ( !dc->deviceList )
	{
		V2MP_DeviceCollection_DeinitAndFree(dc);
		dc = NULL;
	}

	return dc;
}

void V2MP_DeviceCollection_DeinitAndFree(V2MP_DeviceCollection* dc)
{
	if ( !dc )
	{
		return;
	}

	V2MPSC_DoubleLL_DeinitAndFree(dc->deviceList);

	BASEUTIL_FREE(dc);
}

struct V2MP_Device* V2MP_DeviceCollection_CreateDevice(V2MP_DeviceCollection* dc)
{
	V2MPSC_DoubleLL_Node* node;

	if ( !dc )
	{
		return NULL;
	}

	do
	{
		DeviceEntry* entry = NULL;

		node = V2MPSC_DoubleLL_AppendToTail(dc->deviceList);

		if ( !node )
		{
			break;
		}

		entry = (DeviceEntry*)V2MP_DoubleLLNode_GetPayload(node);
		entry->device = V2MP_Device_AllocateAndInit();

		if ( !entry->device )
		{
			break;
		}

		V2MP_Device_SetOwnerNode(entry->device, node);

		return entry->device;
	}
	while ( false );

	if ( node )
	{
		V2MP_DoubleLLNode_Destroy(node);
	}

	return NULL;
}

bool V2MP_DeviceCollection_DestroyDevice(V2MP_DeviceCollection* dc, struct V2MP_Device* device)
{
	V2MPSC_DoubleLL_Node* node;

	if ( !dc || !device )
	{
		return false;
	}

	node = V2MP_Device_GetOwnerNode(device);

	if ( !node || V2MP_DoubleLLNode_GetOwnerList(node) != dc->deviceList )
	{
		return false;
	}

	// This will clean up the contents of the device entry.
	V2MP_DoubleLLNode_Destroy(node);

	return true;
}

size_t V2MP_DeviceCollection_GetDeviceCount(const V2MP_DeviceCollection* dc)
{
	return dc ? V2MPSC_DoubleLL_GetNodeCount(dc->deviceList) : 0;
}

struct V2MP_Device* V2MP_DeviceCollection_GetFirstDevice(const V2MP_DeviceCollection* dc)
{
	if ( !dc )
	{
		return NULL;
	}

	return GetNextDevice(dc, NULL);
}

struct V2MP_Device* V2MP_DeviceCollection_GetNext(const V2MP_DeviceCollection* dc, struct V2MP_Device* device)
{
	if ( !dc || !device )
	{
		return NULL;
	}

	return GetNextDevice(dc, device);
}
