#ifndef V2MP_MODULES_DEVICE_H
#define V2MP_MODULES_DEVICE_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_Device V2MP_Device;
struct V2MP_DevicePort;

typedef struct V2MP_Device_ExtInterface
{
	void* userData;

	void (*poll)(void* userData, V2MP_Device* device);
} V2MP_Device_ExtInterface;

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device);
struct V2MP_DevicePort* V2MP_Device_GetConnectedPort(const V2MP_Device* device);

void V2MP_Device_SetExtInterface(V2MP_Device* device, const V2MP_Device_ExtInterface* interface);
void V2MP_Device_ClearExtInterface(V2MP_Device* device);

void V2MP_Device_Poll(V2MP_Device* device);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICE_H
