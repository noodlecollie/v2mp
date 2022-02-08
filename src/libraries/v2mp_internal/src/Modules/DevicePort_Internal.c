#include "Modules/DevicePort_Internal.h"
#include "Modules/Device_Internal.h"
#include "BaseUtil/Heap.h"

V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void)
{
	return BASEUTIL_CALLOC_STRUCT(V2MP_DevicePort);
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

	BASEUTIL_FREE(port);
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
	if ( !port || port->mailboxController != V2MP_DPMC_DEVICE )
	{
		return false;
	}

	V2MP_CircularBuffer_DeinitAndFree(port->mailbox);
	port->mailbox = NULL;

	port->mailboxStateWhenDeviceRelinquished = V2MP_DPMS_UNAVAILABLE;

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
	if ( !port || !port->mailbox || port->mailboxController != V2MP_DPMC_DEVICE )
	{
		return false;
	}

	if ( V2MP_CircularBuffer_IsEmpty(port->mailbox) )
	{
		port->mailboxStateWhenDeviceRelinquished = V2MP_DPMS_WRITEABLE;
	}
	else
	{
		port->mailboxStateWhenDeviceRelinquished = V2MP_DPMS_READABLE;
	}

	port->mailboxController = V2MP_DPMC_PROGRAM;

	return true;
}

bool V2MP_DevicePort_ProgramRelinquishMailbox(V2MP_DevicePort* port)
{
	if ( !port || !port->mailbox || port->mailboxController != V2MP_DPMC_PROGRAM )
	{
		return false;
	}

	if ( port->mailboxStateWhenDeviceRelinquished == V2MP_DPMS_READABLE )
	{
		// Clear any remaining unread bytes from the mailbox.
		V2MP_CircularBuffer_Reset(port->mailbox);
	}

	port->mailboxStateWhenDeviceRelinquished = V2MP_DPMS_UNAVAILABLE;
	port->mailboxController = V2MP_DPMC_DEVICE;

	return true;
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
