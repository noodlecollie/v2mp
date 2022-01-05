#ifndef V2MP_MODULES_CPU_DEVICEPORTQUERIES_H
#define V2MP_MODULES_CPU_DEVICEPORTQUERIES_H

#include <stdbool.h>
#include "Modules/CPU_Internal.h"

bool V2MP_CPU_DPQ_PortIsConnected(V2MP_CPU* cpu);
bool V2MP_CPU_DPQ_PortIsReadableAndNotBusy(V2MP_CPU* cpu);
bool V2MP_CPU_DPQ_PortIsWriteableAndNotBusy(V2MP_CPU* cpu);
bool V2MP_CPU_DPQ_PortIsExhausted(V2MP_CPU* cpu);
bool V2MP_CPU_DPQ_PortIsBusy(V2MP_CPU* cpu);
bool V2MP_CPU_DPQ_PortIsControlledByProgram(V2MP_CPU* cpu);

#endif // V2MP_MODULES_CPU_DEVICEPORTQUERIES_H
