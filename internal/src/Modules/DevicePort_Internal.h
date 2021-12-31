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
	V2MP_MailboxController mailboxController;
	V2MP_Device* connectedDevice;
};

V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void);
void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port);

void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address);
void V2MP_DevicePort_NotifyDeviceDisconnected(V2MP_DevicePort* port);

#endif // V2MP_MODULES_DEVICEPORT_INTERNAL_H
