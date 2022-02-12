#ifndef V2MP_MODULES_DEVICEPORT_H
#define V2MP_MODULES_DEVICEPORT_H

#include <stdbool.h>
#include <stddef.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DevicePort V2MP_DevicePort;
struct V2MPSC_CircularBuffer;
struct V2MP_Device;

V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port);

// Returns true if the device was connected, or false if there was already
// a different device connected to the port.
bool V2MP_DevicePort_ConnectDevice(V2MP_DevicePort* port, struct V2MP_Device* device);

// Returns true if the current device was disconnected, or false if there
// was no device connected to the port.
bool V2MP_DevicePort_DisconnectDevice(V2MP_DevicePort* port);

bool V2MP_DevicePort_HasConnectedDevice(const V2MP_DevicePort* port);
struct V2MP_Device* V2MP_DevicePort_GetConnectedDevice(const V2MP_DevicePort* port);

bool V2MP_DevicePort_HasMailbox(const V2MP_DevicePort* port);
bool V2MP_DevicePort_IsMailboxEmpty(const V2MP_DevicePort* port);
bool V2MP_DevicePort_IsMailboxFull(const V2MP_DevicePort* port);
size_t V2MP_DevicePort_MailboxCapacity(const V2MP_DevicePort* port);
size_t V2MP_DevicePort_MailboxBytesFree(const V2MP_DevicePort* port);
size_t V2MP_DevicePort_MailboxBytesUsed(const V2MP_DevicePort* port);
V2MP_DeviceMailboxController V2MP_DevicePort_GetMailboxController(const V2MP_DevicePort* port);
bool V2MP_DevicePort_IsMailboxBusy(const V2MP_DevicePort* port);
V2MP_DevicePortMailboxState V2MP_DevicePort_GetMailboxState(const V2MP_DevicePort* port);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICEPORT_H
