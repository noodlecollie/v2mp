#ifndef V2MP_MODULES_SUPERVISOR_ACTION_STACK_H
#define V2MP_MODULES_SUPERVISOR_ACTION_STACK_H

#include <stdbool.h>
#include "LibV2MPInternal/Defs.h"
#include "LibV2MPInternal/Modules/Supervisor.h"

bool V2MP_Supervisor_PerformStackPush(V2MP_Supervisor* supervisor, const V2MP_Word* inWords, size_t numWords);
bool V2MP_Supervisor_PerformStackPop(V2MP_Supervisor* supervisor, V2MP_Word* outWords, size_t numWords);

#endif // V2MP_MODULES_SUPERVISOR_ACTION_STACK_H
