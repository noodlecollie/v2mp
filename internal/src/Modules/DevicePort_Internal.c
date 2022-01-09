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

void V2MP_DevicePort_NotifyMailboxReadyForInteraction(V2MP_DevicePort* port)
{
	if ( !port || !port->connectedDevice )
	{
		return;
	}

	V2MP_Device_NotifyMailboxReadyForInteraction(port->connectedDevice);
}

bool V2MP_DevicePort_DeviceAllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes)
{
	if ( !port || port->mailboxController != V2MP_DMBC_DEVICE )
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

bool V2MP_DevicePort_DeviceRelinquishMailbox(V2MP_DevicePort* port)
{
	if ( !port || !port->mailbox || port->mailboxController != V2MP_DMBC_DEVICE )
	{
		return false;
	}

	port->mailboxWasReadableWhenDeviceTookControl = !V2MP_CircularBuffer_IsEmpty(port->mailbox);
	port->mailboxController = V2MP_DMBC_PROGRAM;
	return true;
}

void V2MP_DevicePort_SetMailboxController(V2MP_DevicePort* port, V2MP_DeviceMailboxController controller)
{
	if ( !port )
	{
		return;
	}

	port->mailboxController = controller;
}

void V2MP_DevicePort_SetMailboxBusy(V2MP_DevicePort* port, bool isBusy)
{
	if ( !port )
	{
		return;
	}

	port->mailboxBusy = isBusy;
}

struct V2MP_CircularBuffer* V2MP_DevicePort_GetMailbox(V2MP_DevicePort* port)
{
	return (struct V2MP_CircularBuffer*)V2MP_DevicePort_GetConstMailbox(port);
}

const struct V2MP_CircularBuffer* V2MP_DevicePort_GetConstMailbox(const V2MP_DevicePort* port)
{
	return port ? port->mailbox : NULL;
}
