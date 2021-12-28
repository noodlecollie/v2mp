#ifndef V2MP_MODULES_CPU_DEVICEPORTOPERATIONS_H
#define V2MP_MODULES_CPU_DEVICEPORTOPERATIONS_H

#include <stdbool.h>
#include "Modules/CPU_Internal.h"

bool V2MP_CPU_DPO_UsableByteCount(V2MP_CPU* cpu);
bool V2MP_CPU_DPO_RelinquishMailbox(V2MP_CPU* cpu);
bool V2MP_CPU_DPO_Read(V2MP_CPU* cpu);
bool V2MP_CPU_DPO_Write(V2MP_CPU* cpu);

#endif // V2MP_MODULES_CPU_DEVICEPORTOPERATIONS_H
