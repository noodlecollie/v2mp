#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Util/Heap.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "Modules/DevicePort_Internal.h"
#include "Modules/Device_Internal.h"

V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_DevicePort);
}

void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port)
{
	if ( !port )
	{
		return;
	}

	V2MP_CircularBuffer_DeinitAndFree(port->mailbox);

	V2MP_FREE(port);
}

V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port)
{
	return port ? port->address : 0;
}

bool V2MP_DevicePort_AllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes)
{
	if ( !port )
	{
		return false;
	}

	V2MP_CircularBuffer_DeinitAndFree(port->mailbox);
	port->mailbox = NULL;

	if ( sizeInBytes < 1 )
	{
		return true;
	}

	port->mailbox = V2MP_CircularBuffer_AllocateAndInit(sizeInBytes);

	return port->mailbox != NULL;
}

void V2MP_DevicePort_DeallocateMailbox(V2MP_DevicePort* port)
{
	V2MP_DevicePort_AllocateMailbox(port, 0);
}

struct V2MP_CircularBuffer* V2MP_DevicePort_GetMailbox(V2MP_DevicePort* port)
{
	return port ? port->mailbox : NULL;
}

bool V2MP_DevicePort_ConnectDevice(V2MP_DevicePort* port, struct V2MP_Device* device)
{
	if ( !port || !device || port->connectedDevice )
	{
		return false;
	}

	port->connectedDevice = device;
	V2MP_Device_NotifyConnectedToPort(device, port->address);

	return true;
}

bool V2MP_DevicePort_DisconnectDevice(V2MP_DevicePort* port)
{
	if ( !port || !port->connectedDevice )
	{
		return false;
	}

	V2MP_Device_NotifyDisconnectedFromPort(port->connectedDevice);
	port->connectedDevice = NULL;

	return true;
}

bool V2MP_DevicePort_IsDeviceConnected(const V2MP_DevicePort* port)
{
	return port ? port->connectedDevice != NULL : false;
}

struct V2MP_Device* V2MP_DevicePort_ConnectedDevice(const V2MP_DevicePort* port)
{
	return port ? port->connectedDevice : NULL;
}
