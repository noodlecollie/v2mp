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
