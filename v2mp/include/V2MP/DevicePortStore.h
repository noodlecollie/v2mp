#ifndef V2MP_DEVICEPORTSTORE_H
#define V2MP_DEVICEPORTSTORE_H

#include <stdbool.h>
#include "V2MP/LibExport.h"
#include "V2MP/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DevicePortStore V2MP_DevicePortStore;
struct V2MP_DevicePort;

API_V2MP size_t V2MP_DevicePortStore_Footprint(void);
API_V2MP V2MP_DevicePortStore* V2MP_DevicePortStore_AllocateAndInit(void);
API_V2MP void V2MP_DevicePortStore_DeinitAndFree(V2MP_DevicePortStore* store);

// Returns NULL if the port could not be created, or if there was already a port at the given address.
API_V2MP struct V2MP_DevicePort* V2MP_DevicePortStore_CreatePort(V2MP_DevicePortStore* store, V2MP_Word address);

// Returns true if a port at the given address was destroyed, or false if there was no port at this address.
API_V2MP bool V2MP_DevicePortStore_DestroyPort(V2MP_DevicePortStore* store, V2MP_Word address);

API_V2MP struct V2MP_DevicePort* V2MP_DevicePort_GetPort(V2MP_DevicePortStore* store, V2MP_Word address);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_DEVICEPORTSTORE_H
