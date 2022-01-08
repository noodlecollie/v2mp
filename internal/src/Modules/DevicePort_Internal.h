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
	V2MP_DeviceMailboxController mailboxController;
	V2MP_Device* connectedDevice;
	bool mailboxWasReadableWhenDeviceTookControl;
};

V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void);
void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port);

void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address);
void V2MP_DevicePort_NotifyDeviceDisconnected(V2MP_DevicePort* port);

// If any mailbox already exists, its contents are discarded.
// Specifiying a mailbox of 0 bytes simply deallocates its memory.
// The port must be controlled by the device for the mailbox to be allocated.
bool V2MP_DevicePort_DeviceAllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes);
bool V2MP_DevicePort_DeviceDeallocateMailbox(V2MP_DevicePort* port);

bool V2MP_DevicePort_DeviceRelinquishMailbox(V2MP_DevicePort* port);

bool V2MP_DevicePort_PassMailboxControlToDevice(V2MP_DevicePort* port);
void V2MP_DevicePort_PassMailboxControlToSupervisor(V2MP_DevicePort* port);
void V2MP_DevicePort_PassMailboxControlToProgram(V2MP_DevicePort* port);

struct V2MP_CircularBuffer* V2MP_DevicePort_GetMailbox(V2MP_DevicePort* port);
const struct V2MP_CircularBuffer* V2MP_DevicePort_GetConstMailbox(const V2MP_DevicePort* port);

#endif // V2MP_MODULES_DEVICEPORT_INTERNAL_H
