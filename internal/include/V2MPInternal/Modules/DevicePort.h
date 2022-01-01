#ifndef V2MP_MODULES_DEVICEPORT_H
#define V2MP_MODULES_DEVICEPORT_H

#include <stdbool.h>
#include <stddef.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DevicePort V2MP_DevicePort;
struct V2MP_CircularBuffer;
struct V2MP_Device;

V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port);

struct V2MP_CircularBuffer* V2MP_DevicePort_GetMailbox(V2MP_DevicePort* port);
const struct V2MP_CircularBuffer* V2MP_DevicePort_GetConstMailbox(const V2MP_DevicePort* port);

// Returns true if the device was connected, or false if there was already
// a different device connected to the port.
bool V2MP_DevicePort_ConnectDevice(V2MP_DevicePort* port, struct V2MP_Device* device);

// Returns true if the current device was disconnected, or false if there
// was no device connected to the port.
bool V2MP_DevicePort_DisconnectDevice(V2MP_DevicePort* port);

bool V2MP_DevicePort_HasConnectedDevice(const V2MP_DevicePort* port);
struct V2MP_Device* V2MP_DevicePort_GetConnectedDevice(const V2MP_DevicePort* port);

V2MP_MailboxController V2MP_DevicePort_GetMailboxController(const V2MP_DevicePort* port);
void V2MP_DevicePort_SetMailboxController(V2MP_DevicePort* port, V2MP_MailboxController controller);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICEPORT_H
