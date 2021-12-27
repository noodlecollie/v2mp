#ifndef V2MP_MODULES_SUPERVISOR_H
#define V2MP_MODULES_SUPERVISOR_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_Supervisor V2MP_Supervisor;
struct V2MP_Mainboard;

V2MP_Supervisor* V2MP_Supervisor_AllocateAndInit(void);
void V2MP_Supervisor_DeinitAndFree(V2MP_Supervisor* supervisor);

struct V2MP_Mainboard* V2MP_Supervisor_GetMainboard(const V2MP_Supervisor* supervisor);
void V2MP_Supervisor_SetMainboard(V2MP_Supervisor* supervisor, struct V2MP_Mainboard* mainboard);

bool V2MP_Supervisor_ExecuteClockCycle(V2MP_Supervisor* supervisor);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_SUPERVISOR_H
