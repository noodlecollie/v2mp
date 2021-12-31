#include "V2MPInternal/Modules/Device.h"
#include "V2MPInternal/Util/Util.h"
#include "Modules/Device_Internal.h"

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device)
{
	return device ? device->connectedPort != NULL : false;
}

struct V2MP_DevicePort* V2MP_Device_GetConnectedPort(const V2MP_Device* device)
{
	return device ? device->connectedPort : NULL;
}

void V2MP_Device_SetExtInterface(V2MP_Device* device, const V2MP_Device_ExtInterface* interface)
{
	if ( !device )
	{
		return;
	}

	if ( interface )
	{
		device->extInterface = *interface;
	}
	else
	{
		V2MP_ZERO_STRUCT_PTR(&device->extInterface);
	}
}

void V2MP_Device_ClearExtInterface(V2MP_Device* device)
{
	V2MP_Device_SetExtInterface(device, NULL);
}

void V2MP_Device_Poll(V2MP_Device* device)
{
	if ( !device || !device->extInterface.poll )
	{
		return;
	}

	device->extInterface.poll(device->extInterface.userData, device);
}
