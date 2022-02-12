#include "Modules/Device_Internal.h"
#include "Modules/DevicePort_Internal.h"
#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"

V2MP_Device* V2MP_Device_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MP_Device);
}

void V2MP_Device_DeinitAndFree(V2MP_Device* device)
{
	if ( !device )
	{
		return;
	}

	if ( device->callbacks.onDeviceAboutToBeDestroyed )
	{
		device->callbacks.onDeviceAboutToBeDestroyed(device->callbacks.userData, device);
		BASEUTIL_ZERO_STRUCT_PTR(&device->callbacks);
	}

	if ( device->connectedPort )
	{
		V2MP_DevicePort_NotifyDeviceDisconnected(device->connectedPort);
		device->connectedPort = NULL;
	}

	BASEUTIL_FREE(device);
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

bool V2MP_Device_NotifyMailboxReadyForInteraction(V2MP_Device* device)
{
	if ( !device || !device->callbacks.onMailboxReadyForInteraction )
	{
		return false;
	}

	device->callbacks.onMailboxReadyForInteraction(device->callbacks.userData, device);
	return true;
}

V2MPSC_DoubleLL_Node* V2MP_Device_GetOwnerNode(const V2MP_Device* device)
{
	return device ? device->ownerNode : NULL;
}

void V2MP_Device_SetOwnerNode(V2MP_Device* device, V2MPSC_DoubleLL_Node* node)
{
	if ( !device )
	{
		return;
	}

	device->ownerNode = node;
}

bool V2MP_Device_Poll(V2MP_Device* device)
{
	if ( !device || !device->callbacks.onPoll )
	{
		return false;
	}

	device->callbacks.onPoll(device->callbacks.userData, device);
	return true;
}
