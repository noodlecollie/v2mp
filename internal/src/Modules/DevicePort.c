#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "Modules/DevicePort_Internal.h"
#include "Modules/Device_Internal.h"

V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port)
{
	return port ? port->address : 0;
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

struct V2MP_CircularBuffer* V2MP_DevicePort_GetMailbox(V2MP_DevicePort* port)
{
	return (struct V2MP_CircularBuffer*)V2MP_DevicePort_GetConstMailbox(port);
}

const struct V2MP_CircularBuffer* V2MP_DevicePort_GetConstMailbox(const V2MP_DevicePort* port)
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

V2MP_MailboxController V2MP_DevicePort_GetMailboxController(const V2MP_DevicePort* port)
{
	return port ? port->mailboxController : V2MP_MBC_DEVICE;
}

void V2MP_DevicePort_SetMailboxController(V2MP_DevicePort* port, V2MP_MailboxController controller)
{
	if ( !port )
	{
		return;
	}

	port->mailboxController = controller;
}
