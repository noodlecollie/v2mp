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

V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void);
void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port);

V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port);
void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address);

// If any mailbox already exists, its contents are discarded.
// Specifiying a mailbox of 0 bytes simply deallocates its memory.
bool V2MP_DevicePort_AllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes);
void V2MP_DevicePort_DeallocateMailbox(V2MP_DevicePort* port);
struct V2MP_CircularBuffer* V2MP_DevicePort_GetMailbox(V2MP_DevicePort* port);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICEPORT_H
