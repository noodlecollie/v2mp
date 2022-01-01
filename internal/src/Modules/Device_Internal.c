#include "Modules/Device_Internal.h"
#include "Modules/DevicePort_Internal.h"
#include "V2MPInternal/Util/Heap.h"
#include "V2MPInternal/Util/Util.h"

V2MP_Device* V2MP_Device_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_Device);
}

void V2MP_Device_DeinitAndFree(V2MP_Device* device)
{
	if ( !device )
	{
		return;
	}

	if ( device->extInterface.onDeviceAboutToBeDestroyed )
	{
		device->extInterface.onDeviceAboutToBeDestroyed(device->extInterface.userData, device);
		V2MP_ZERO_STRUCT_PTR(&device->extInterface);
	}

	if ( device->connectedPort )
	{
		V2MP_DevicePort_NotifyDeviceDisconnected(device->connectedPort);
		device->connectedPort = NULL;
	}

	V2MP_FREE(device);
}

void V2MP_Device_NotifyConnectedToPort(V2MP_Device* device, struct V2MP_DevicePort* port)
{
	if ( !device )
	{
		return;
	}

	device->connectedPort = port;
}

void V2MP_Device_NotifyDisconnectedFromPort(V2MP_Device* device)
{
	if ( !device )
	{
		return;
	}

	device->connectedPort = NULL;
}

bool V2MP_Device_NotifyMailboxControlAcquired(V2MP_Device* device)
{
	if ( !device || !device->extInterface.onMailboxControlAcquired )
	{
		return false;
	}

	device->extInterface.onMailboxControlAcquired(device->extInterface.userData, device);
	return true;
}

V2MP_DoubleLL_Node* V2MP_Device_GetOwnerNode(const V2MP_Device* device)
{
	return device ? device->ownerNode : NULL;
}

void V2MP_Device_SetOwnerNode(V2MP_Device* device, V2MP_DoubleLL_Node* node)
{
	if ( !device )
	{
		return;
	}

	device->ownerNode = node;
}
