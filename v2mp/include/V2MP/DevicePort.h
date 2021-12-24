#ifndef V2MP_DEVICEPORT_H
#define V2MP_DEVICEPORT_H

#include <stddef.h>
#include <stdbool.h>
#include "V2MP/LibExport.h"
#include "V2MP/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DevicePort V2MP_DevicePort;

API_V2MP size_t V2MP_DevicePort_Footprint(void);
API_V2MP V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void);
API_V2MP void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port);

API_V2MP size_t V2MP_DevicePort_GetMailboxSize(V2MP_DevicePort* port);
API_V2MP size_t V2MP_DevicePort_GetNumberOfBytesInMailbox(V2MP_DevicePort* port);

// If any mailbox already exists, its contents are discarded.
// Specifiying a mailbox of 0 bytes simply deallocates its memory.
API_V2MP bool V2MP_DevicePort_AllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes);
API_V2MP void V2MP_DevicePort_DeallocateMailbox(V2MP_DevicePort* port);

// These functions reurn the actual number of bytes written to or read from the mailbox.
API_V2MP size_t V2MP_DevicePort_WriteToMailbox(V2MP_DevicePort* port, const V2MP_Byte* data, size_t dataSize);
API_V2MP size_t V2MP_DevicePort_ReadFromMailbox(V2MP_DevicePort* port, V2MP_Byte* buffer, size_t bufferSize);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_DEVICEPORT_H
