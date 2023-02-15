#include "LibV2MPInternal/Modules/VirtualMachine.h"
#include "LibV2MPInternal/Modules/Mainboard.h"
#include "LibV2MPInternal/Modules/Supervisor.h"
#include "LibV2MPInternal/Modules/MemoryStore.h"
#include "LibBaseUtil/Heap.h"

struct V2MP_VirtualMachine
{
	V2MP_Mainboard* mainboard;
	V2MP_Supervisor* supervisor;
};

V2MP_VirtualMachine* V2MP_VirtualMachine_AllocateAndInit(void)
{
	V2MP_VirtualMachine* vm = BASEUTIL_CALLOC_STRUCT(V2MP_VirtualMachine);

	vm->mainboard = V2MP_Mainboard_AllocateAndInit();
	vm->supervisor = V2MP_Supervisor_AllocateAndInit();

	if ( !vm->mainboard || !vm->supervisor )
	{
		V2MP_VirtualMachine_DeinitAndFree(vm);
		return NULL;
	}

	V2MP_Supervisor_SetMainboard(vm->supervisor, vm->mainboard);

	return vm;
}

void V2MP_VirtualMachine_DeinitAndFree(V2MP_VirtualMachine* vm)
{
	if ( !vm )
	{
		return;
	}

	V2MP_Supervisor_DeinitAndFree(vm->supervisor);
	V2MP_Mainboard_DeinitAndFree(vm->mainboard);

	BASEUTIL_FREE(vm);
}

struct V2MP_Mainboard* V2MP_VirtualMachine_GetMainboard(V2MP_VirtualMachine* vm)
{
	return vm ? vm->mainboard : NULL;
}

struct V2MP_Supervisor* V2MP_VirtualMachine_GetSupervisor(V2MP_VirtualMachine* vm)
{
	return vm ? vm->supervisor : NULL;
}

size_t V2MP_VirtualMachine_GetTotalMemoryBytes(V2MP_VirtualMachine* vm)
{
	V2MP_MemoryStore* memoryStore;

	if ( !vm )
	{
		return 0;
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(vm->mainboard);

	return V2MP_MemoryStore_GetTotalMemorySize(memoryStore);
}

bool V2MP_VirtualMachine_AllocateTotalMemory(V2MP_VirtualMachine* vm, size_t sizeInBytes)
{
	V2MP_MemoryStore* memoryStore;

	if ( !vm )
	{
		return false;
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(vm->mainboard);

	return V2MP_MemoryStore_AllocateTotalMemory(memoryStore, sizeInBytes);
}

bool V2MP_VirtualMachine_LoadProgram(
	V2MP_VirtualMachine* vm,
	const V2MP_Word* cs,
	size_t csLengthInWords,
	const V2MP_Word* ds,
	size_t dsLengthInWords,
	size_t ssLengthInWords
)
{
	return vm->supervisor
		? V2MP_Supervisor_LoadProgram(vm->supervisor, cs, csLengthInWords, ds, dsLengthInWords, ssLengthInWords)
		: false;
}

void V2MP_VirtualMachine_ClearProgram(V2MP_VirtualMachine* vm)
{
	if ( !vm )
	{
		return;
	}

	V2MP_Supervisor_ClearProgram(vm->supervisor);
}

bool V2MP_VirtualMachine_IsProgramLoaded(const V2MP_VirtualMachine* vm)
{
	return vm
		? V2MP_Supervisor_IsProgramLoaded(vm->supervisor)
		: false;
}

bool V2MP_VirtualMachine_ExecuteClockCycle(V2MP_VirtualMachine* vm)
{
	return vm
		? V2MP_Supervisor_ExecuteClockCycle(vm->supervisor)
		: false;
}
