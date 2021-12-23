#ifndef V2MP_DEVICEPORT_H
#define V2MP_DEVICEPORT_H

#include <stddef.h>
#include "V2MP/LibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DevicePort V2MP_DevicePort;

API_V2MP size_t V2MP_DevicePort_Footprint(void);
API_V2MP V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void);
API_V2MP void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_DEVICEPORT_H
