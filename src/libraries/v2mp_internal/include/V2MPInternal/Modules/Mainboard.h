#ifndef V2MP_MODULES_MAINBOARD_H
#define V2MP_MODULES_MAINBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_Mainboard V2MP_Mainboard;
struct V2MP_CPU;
struct V2MP_MemoryStore;
struct V2MP_DevicePortCollection;
struct V2MP_DeviceCollection;

V2MP_Mainboard* V2MP_Mainboard_AllocateAndInit(void);
void V2MP_Mainboard_DeinitAndFree(V2MP_Mainboard* board);

struct V2MP_CPU* V2MP_Mainboard_GetCPU(const V2MP_Mainboard* board);
struct V2MP_MemoryStore* V2MP_Mainboard_GetMemoryStore(const V2MP_Mainboard* board);
struct V2MP_DevicePortCollection* V2MP_Mainboard_GetDevicePortCollection(const V2MP_Mainboard* board);
struct V2MP_DeviceCollection* V2MP_Mainboard_GetDeviceCollection(const V2MP_Mainboard* board);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_MAINBOARD_H
