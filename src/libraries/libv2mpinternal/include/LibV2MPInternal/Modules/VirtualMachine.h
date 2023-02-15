#ifndef V2MP_MODULES_VIRTUALMACHINE_H
#define V2MP_MODULES_VIRTUALMACHINE_H

#include <stdbool.h>
#include <stddef.h>
#include "LibV2MPInternal/Defs.h"

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

size_t V2MP_VirtualMachine_GetTotalMemoryBytes(V2MP_VirtualMachine* vm);
bool V2MP_VirtualMachine_AllocateTotalMemory(V2MP_VirtualMachine* vm, size_t sizeInBytes);

bool V2MP_VirtualMachine_LoadProgram(
	V2MP_VirtualMachine* vm,
	const V2MP_Word* cs,
	size_t csLengthInWords,
	const V2MP_Word* ds,
	size_t dsLengthInWords,
	size_t ssLengthInWords
);

void V2MP_VirtualMachine_ClearProgram(V2MP_VirtualMachine* vm);
bool V2MP_VirtualMachine_IsProgramLoaded(const V2MP_VirtualMachine* vm);

bool V2MP_VirtualMachine_ExecuteClockCycle(V2MP_VirtualMachine* vm);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_VIRTUALMACHINE_H
