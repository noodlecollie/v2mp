#ifndef V2MP_MODULES_DEVICEPORTCOLLECTION_H
#define V2MP_MODULES_DEVICEPORTCOLLECTION_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DevicePortCollection V2MP_DevicePortCollection;
struct V2MP_DevicePort;

V2MP_DevicePortCollection* V2MP_DevicePortCollection_AllocateAndInit(void);
void V2MP_DevicePortCollection_DeinitAndFree(V2MP_DevicePortCollection* dpc);

// Returns NULL if the port could not be created, or if there was already a port at the given address.
struct V2MP_DevicePort* V2MP_DevicePortCollection_CreatePort(V2MP_DevicePortCollection* dpc, V2MP_Word address);

// Returns true if a port at the given address was destroyed, or false if there was no port at this address.
bool V2MP_DevicePortCollection_DestroyPort(V2MP_DevicePortCollection* dpc, V2MP_Word address);

struct V2MP_DevicePort* V2MP_DevicePortCollection_GetPort(V2MP_DevicePortCollection* dpc, V2MP_Word address);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_DEVICEPORTCOLLECTION_H
