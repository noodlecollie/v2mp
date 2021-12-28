#ifndef V2MP_MODULES_VIRTUALMACHINE_H
#define V2MP_MODULES_VIRTUALMACHINE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_VirtualMachine V2MP_VirtualMachine;
struct V2MP_Supervisor;
struct V2MP_Mainboard;

V2MP_VirtualMachine* V2MP_VirtualMachine_AllocateAndInit(void);
void V2MP_VirtualMachine_DeinitAndFree(V2MP_VirtualMachine* vm);

struct V2MP_Mainboard* V2MP_VirtualMachine_GetMainboard(V2MP_VirtualMachine* vm);
struct V2MP_Supervisor* V2MP_VirtualMachine_GetSupervisor(V2MP_VirtualMachine* vm);

size_t V2MP_VirtualMachile_GetTotalMemoryBytes(V2MP_VirtualMachine* vm);
bool V2MP_VirtualMachine_AllocateTotalMemory(V2MP_VirtualMachine* vm, size_t sizeInBytes);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_VIRTUALMACHINE_H
