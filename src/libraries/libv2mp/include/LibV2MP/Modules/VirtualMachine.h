#ifndef V2MPINTERNAL_MODULES_VIRTUALMACHINE_H
#define V2MPINTERNAL_MODULES_VIRTUALMACHINE_H

#include <stdbool.h>
#include <stddef.h>
#include "LibV2MP/LibExport.gen.h"
#include "LibV2MP/Defs.h"

typedef struct V2MP_VirtualMachine V2MP_VirtualMachine;
struct V2MP_Supervisor;
struct V2MP_Mainboard;

LIBV2MP_PUBLIC(V2MP_VirtualMachine*) V2MP_VirtualMachine_AllocateAndInit(void);
LIBV2MP_PUBLIC(void) V2MP_VirtualMachine_DeinitAndFree(V2MP_VirtualMachine* vm);

LIBV2MP_PUBLIC(struct V2MP_Mainboard*) V2MP_VirtualMachine_GetMainboard(V2MP_VirtualMachine* vm);
LIBV2MP_PUBLIC(struct V2MP_Supervisor*) V2MP_VirtualMachine_GetSupervisor(V2MP_VirtualMachine* vm);

LIBV2MP_PUBLIC(size_t) V2MP_VirtualMachine_GetTotalMemoryBytes(V2MP_VirtualMachine* vm);
LIBV2MP_PUBLIC(bool) V2MP_VirtualMachine_AllocateTotalMemory(V2MP_VirtualMachine* vm, size_t sizeInBytes);

LIBV2MP_PUBLIC(bool) V2MP_VirtualMachine_LoadProgram(
	V2MP_VirtualMachine* vm,
	const V2MP_Word* cs,
	size_t csLengthInWords,
	const V2MP_Word* ds,
	size_t dsLengthInWords,
	size_t ssLengthInWords
);

LIBV2MP_PUBLIC(void) V2MP_VirtualMachine_ClearProgram(V2MP_VirtualMachine* vm);
LIBV2MP_PUBLIC(bool) V2MP_VirtualMachine_IsProgramLoaded(const V2MP_VirtualMachine* vm);

LIBV2MP_PUBLIC(bool) V2MP_VirtualMachine_ExecuteClockCycle(V2MP_VirtualMachine* vm);

#endif // V2MPINTERNAL_MODULES_VIRTUALMACHINE_H
