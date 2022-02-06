#ifndef V2MP_MODULES_SUPERVISOR_ACTION_DPQ_H
#define V2MP_MODULES_SUPERVISOR_ACTION_DPQ_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/Supervisor.h"

bool V2MP_Supervisor_HandlePerformDevicePortQueryImpl(V2MP_Supervisor* supervisor, V2MP_Word port, V2MP_DevicePortQueryType queryType);

#endif // V2MP_MODULES_SUPERVISOR_ACTION_DPQ_H
