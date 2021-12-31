#ifndef V2MP_MODULES_DEVICE_H
#define V2MP_MODULES_DEVICE_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_Device V2MP_Device;

V2MP_Device* V2MP_Device_AllocateAndInit(void);
void V2MP_Device_DeinitAndFree(V2MP_Device* device);

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device);

// Only valid if the device is connected to a port.
V2MP_Word V2MP_Device_ConnectedPortAddress(const V2MP_Device* device);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICE_H
