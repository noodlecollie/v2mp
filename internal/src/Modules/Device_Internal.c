#include "Modules/Device_Internal.h"

void V2MP_Device_NotifyConnectedToPort(V2MP_Device* device, V2MP_Word address)
{
	if ( !device )
	{
		return;
	}

	device->connectedToPort = true;
	device->portAddress = address;
}

void V2MP_Device_NotifyDisconnectedFromPort(V2MP_Device* device)
{
	if ( !device )
	{
		return;
	}

	device->connectedToPort = false;
	device->portAddress = 0;
}
