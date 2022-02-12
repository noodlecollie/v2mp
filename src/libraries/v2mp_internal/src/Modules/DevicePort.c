#include "V2MPInternal/Modules/DevicePort.h"
#include "SharedComponents/CircularBuffer.h"
#include "Modules/DevicePort_Internal.h"
#include "Modules/Device_Internal.h"

V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port)
{
	return port ? port->address : 0;
}

bool V2MP_DevicePort_ConnectDevice(V2MP_DevicePort* port, struct V2MP_Device* device)
{
	if ( !port || !device || port->connectedDevice )
	{
		return false;
	}

	port->connectedDevice = device;
	V2MP_Device_NotifyConnectedToPort(device, port);

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

bool V2MP_DevicePort_HasConnectedDevice(const V2MP_DevicePort* port)
{
	return port ? port->connectedDevice != NULL : false;
}

struct V2MP_Device* V2MP_DevicePort_GetConnectedDevice(const V2MP_DevicePort* port)
{
	return port ? port->connectedDevice : NULL;
}

bool V2MP_DevicePort_HasMailbox(const V2MP_DevicePort* port)
{
	return port ? port->mailbox != NULL : false;
}

bool V2MP_DevicePort_IsMailboxEmpty(const V2MP_DevicePort* port)
{
	return (port && port->mailbox) ? V2MPSC_CircularBuffer_IsEmpty(port->mailbox) : true;
}

bool V2MP_DevicePort_IsMailboxFull(const V2MP_DevicePort* port)
{
	return (port && port->mailbox) ? V2MPSC_CircularBuffer_IsFull(port->mailbox) : false;
}

size_t V2MP_DevicePort_MailboxCapacity(const V2MP_DevicePort* port)
{
	return (port && port->mailbox) ? V2MPSC_CircularBuffer_Capacity(port->mailbox) : 0;
}

size_t V2MP_DevicePort_MailboxBytesFree(const V2MP_DevicePort* port)
{
	return (port && port->mailbox) ? V2MPSC_CircularBuffer_BytesFree(port->mailbox) : 0;
}

size_t V2MP_DevicePort_MailboxBytesUsed(const V2MP_DevicePort* port)
{
	return (port && port->mailbox) ? V2MPSC_CircularBuffer_BytesUsed(port->mailbox) : 0;
}

V2MP_DeviceMailboxController V2MP_DevicePort_GetMailboxController(const V2MP_DevicePort* port)
{
	return port ? port->mailboxController : V2MP_DPMC_DEVICE;
}

bool V2MP_DevicePort_IsMailboxBusy(const V2MP_DevicePort* port)
{
	return port ? port->mailboxBusy : false;
}

V2MP_DevicePortMailboxState V2MP_DevicePort_GetMailboxState(const V2MP_DevicePort* port)
{
	if ( !port || !port->mailbox || port->mailboxController == V2MP_DPMC_DEVICE )
	{
		return V2MP_DPMS_UNAVAILABLE;
	}

	if ( port->mailboxStateWhenDeviceRelinquished == V2MP_DPMS_READABLE )
	{
		return V2MPSC_CircularBuffer_IsEmpty(port->mailbox)
			? V2MP_DPMS_EXHAUSTED
			: V2MP_DPMS_READABLE;
	}
	else
	{
		return V2MPSC_CircularBuffer_IsFull(port->mailbox)
			? V2MP_DPMS_EXHAUSTED
			: V2MP_DPMS_WRITEABLE;
	}
}
