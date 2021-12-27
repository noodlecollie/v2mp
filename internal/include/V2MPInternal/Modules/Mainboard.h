#ifndef V2MP_MODULES_MAINBOARD_H
#define V2MP_MODULES_MAINBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_Mainboard V2MP_Mainboard;
struct V2MP_CPURenameMe;
struct V2MP_MemoryStoreRenameMe;

V2MP_Mainboard* V2MP_Mainboard_AllocateAndInit(void);
void V2MP_Mainboard_DeinitAndFree(V2MP_Mainboard* board);

struct V2MP_CPURenameMe* V2MP_Mainboard_GetCPU(V2MP_Mainboard* board);
struct V2MP_MemoryStoreRenameMe* V2MP_Mainboard_GetMemoryStore(V2MP_Mainboard* board);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_MAINBOARD_H
