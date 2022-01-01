#include "Modules/DevicePort_Internal.h"
#include "Modules/Device_Internal.h"
#include "V2MPInternal/Util/Heap.h"

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

	if ( port->connectedDevice )
	{
		V2MP_Device_NotifyDisconnectedFromPort(port->connectedDevice);
	}

	V2MP_CircularBuffer_DeinitAndFree(port->mailbox);

	V2MP_FREE(port);
}

void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address)
{
	if ( !port )
	{
		return;
	}

	port->address = address;
}

void V2MP_DevicePort_NotifyDeviceDisconnected(V2MP_DevicePort* port)
{
	if ( !port )
	{
		return;
	}

	port->connectedDevice = NULL;
}

bool V2MP_DevicePort_DeviceAllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes)
{
	if ( !port || port->mailboxController != V2MP_MBC_DEVICE )
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

bool V2MP_DevicePort_DeviceDeallocateMailbox(V2MP_DevicePort* port)
{
	return V2MP_DevicePort_DeviceAllocateMailbox(port, 0);
}
