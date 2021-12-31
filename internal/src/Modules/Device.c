#include "V2MPInternal/Modules/Device.h"
#include "Modules/Device_Internal.h"

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device)
{
	return device ? device->connectedPort != NULL : false;
}

struct V2MP_DevicePort* V2MP_Device_GetConnectedPort(const V2MP_Device* device)
{
	return device ? device->connectedPort : NULL;
}
