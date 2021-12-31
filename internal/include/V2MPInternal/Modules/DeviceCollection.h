#ifndef V2MP_MODULES_DEVICECOLLECTION_H
#define V2MP_MODULES_DEVICECOLLECTION_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DeviceCollection V2MP_DeviceCollection;
struct V2MP_Device;

V2MP_DeviceCollection* V2MP_DeviceCollection_AllocateAndInit(void);
void V2MP_DeviceCollection_DeinitAndFree(V2MP_DeviceCollection* dc);

struct V2MP_Device* V2MP_DeviceCollection_CreateDevice(V2MP_DeviceCollection* dc);
bool V2MP_DeviceCollection_DestroyDevice(V2MP_DeviceCollection* dc, struct V2MP_Device* device);

size_t V2MP_DeviceCollection_GetDeviceCount(const V2MP_DeviceCollection* dc);

struct V2MP_Device* V2MP_DeviceCollection_GetFirstDevice(const V2MP_DeviceCollection* dc);
struct V2MP_Device* V2MP_DeviceCollection_GetNext(const V2MP_DeviceCollection* dc, struct V2MP_Device* device);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICECOLLECTION_H
