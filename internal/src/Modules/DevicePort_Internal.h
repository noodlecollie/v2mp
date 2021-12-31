#ifndef V2MP_MODULES_DEVICEPORT_INTERNAL_H
#define V2MP_MODULES_DEVICEPORT_INTERNAL_H

#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Modules/Device.h"

struct V2MP_DevicePort
{
	V2MP_Word address;
	V2MP_CircularBuffer* mailbox;
	V2MP_Device* connectedDevice;
};

void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address);

#endif // V2MP_MODULES_DEVICEPORT_INTERNAL_H
