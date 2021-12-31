#ifndef V2MP_MODULES_DEVICE_INTERNAL_H
#define V2MP_MODULES_DEVICE_INTERNAL_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/Device.h"

struct V2MP_Device
{
	bool connectedToPort;
	V2MP_Word portAddress;
};

void V2MP_Device_NotifyConnectedToPort(V2MP_Device* device, V2MP_Word address);
void V2MP_Device_NotifyDisconnectedFromPort(V2MP_Device* device);

#endif // V2MP_MODULES_DEVICE_INTERNAL_H
