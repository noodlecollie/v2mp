#ifndef V2MPINTERNAL_MODULES_MAINBOARD_H
#define V2MPINTERNAL_MODULES_MAINBOARD_H

#include "LibV2MP/LibExport.gen.h"

typedef struct V2MP_Mainboard V2MP_Mainboard;
struct V2MP_CPU;
struct V2MP_MemoryStore;

LIBV2MP_PUBLIC(V2MP_Mainboard*) V2MP_Mainboard_AllocateAndInit(void);
LIBV2MP_PUBLIC(void) V2MP_Mainboard_DeinitAndFree(V2MP_Mainboard* board);

LIBV2MP_PUBLIC(struct V2MP_CPU*) V2MP_Mainboard_GetCPU(const V2MP_Mainboard* board);
LIBV2MP_PUBLIC(struct V2MP_MemoryStore*) V2MP_Mainboard_GetMemoryStore(const V2MP_Mainboard* board);

#endif // V2MPINTERNAL_MODULES_MAINBOARD_H
