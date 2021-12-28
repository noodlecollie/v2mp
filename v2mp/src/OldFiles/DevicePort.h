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
struct V2MP_MemoryStore;

typedef enum V2MP_DevicePort_MailboxState
{
	V2MP_DPMS_UNAVAILABLE = 0,
	V2MP_DPMS_READABLE,
	V2MP_DPMS_WRITEABLE,
	V2MP_DPMS_EXHAUSTED
} V2MP_DevicePort_MailboxState;

typedef enum V2MP_DevicePort_MailboxController
{
	V2MP_DPMC_DEVICE = 0,
	V2MP_DPMC_PROGRAM
} V2MP_DevicePort_MailboxController;

API_V2MP size_t V2MP_DevicePort_Footprint(void);
API_V2MP V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void);
API_V2MP void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port);

API_V2MP V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port);
API_V2MP void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address);

API_V2MP size_t V2MP_DevicePort_GetMailboxSize(const V2MP_DevicePort* port);
API_V2MP size_t V2MP_DevicePort_GetMailboxMessageLength(const V2MP_DevicePort* port);
API_V2MP size_t V2MP_DevicePort_GetMailboxFreeSpace(const V2MP_DevicePort* port);
API_V2MP V2MP_DevicePort_MailboxState V2MP_DevicePort_GetMailboxState(const V2MP_DevicePort* port);
API_V2MP V2MP_DevicePort_MailboxController V2MP_DevicePort_GetMailboxController(const V2MP_DevicePort* port);

// Speed is bytes per clock cycle. If zero, this means any amount of data is transferred in one clock cycle.
API_V2MP size_t V2MP_DevicePort_GetMailboxDataTransferSpeed(const V2MP_DevicePort* port);
API_V2MP void V2MP_DevicePort_SetMailboxDataTransferSpeed(V2MP_DevicePort* port, size_t speed);

// If any mailbox already exists, its contents are discarded.
// Specifiying a mailbox of 0 bytes simply deallocates its memory.
API_V2MP bool V2MP_DevicePort_AllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes);
API_V2MP void V2MP_DevicePort_DeallocateMailbox(V2MP_DevicePort* port);

// The memory store is expected to live at least as long as the port.
API_V2MP bool V2MP_DevicePort_BeginReadFromMailbox(
	V2MP_DevicePort* port,
	struct V2MP_MemoryStore* memoryStore,
	V2MP_Word destAddress,
	V2MP_Word maxBytesToRead
);

// The memory store is expected to live at least as long as the port.
API_V2MP bool V2MP_DevicePort_BeginWriteToMailbox(
	V2MP_DevicePort* port,
	struct V2MP_MemoryStore* memoryStore,
	V2MP_Word srcAddress,
	V2MP_Word maxBytesToWrite
);

API_V2MP bool V2MP_DevicePort_ExecuteCycle(V2MP_DevicePort* port, V2MP_Fault* outFault);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_DEVICEPORT_H
