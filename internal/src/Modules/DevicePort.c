#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Components/CircularBuffer.h"
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

V2MP_MailboxController V2MP_DevicePort_GetMailboxController(const V2MP_DevicePort* port)
{
	return port ? port->mailboxController : V2MP_MBC_DEVICE;
}
