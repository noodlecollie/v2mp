#ifndef V2MP_MODULES_DEVICE_H
#define V2MP_MODULES_DEVICE_H

#include <stdbool.h>
#include <stddef.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_Device V2MP_Device;
struct V2MP_DevicePort;

typedef struct V2MP_Device_ExtInterface
{
	void* userData;

	void (*onDeviceAboutToBeDestroyed)(void* userData, V2MP_Device* device);
	void (*onPoll)(void* userData, V2MP_Device* device);
} V2MP_Device_ExtInterface;

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device);
bool V2MP_Device_ControlsConnectedMailbox(const V2MP_Device* device);

// Only valid if the device is connected to a port.
V2MP_Word V2MP_Device_AddressOfConnectedPort(const V2MP_Device* device);

void V2MP_Device_SetExtInterface(V2MP_Device* device, const V2MP_Device_ExtInterface* interface);
void V2MP_Device_ClearExtInterface(V2MP_Device* device);

bool V2MP_Device_Poll(V2MP_Device* device);

// The following only succeed if the connected port is controlled by the device.
bool V2MP_Device_AllocateConnectedMailbox(V2MP_Device* device, size_t sizeInBytes);
bool V2MP_Device_DeallocateConnectedMailbox(V2MP_Device* device);
size_t V2MP_Device_WriteToConnectedMailbox(V2MP_Device* device, const V2MP_Byte* data, size_t dataSize);
size_t V2MP_Device_ReadFromConnectedMailbox(V2MP_Device* device, V2MP_Byte* outBuffer, size_t outBufferSize);

void V2MP_Device_RelinquishConnectedMailbox(V2MP_Device* device);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICE_H
