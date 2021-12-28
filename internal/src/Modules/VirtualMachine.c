#include "V2MPInternal/Modules/VirtualMachine.h"
#include "V2MPInternal/Modules/Mainboard.h"
#include "V2MPInternal/Modules/Supervisor.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Util/Heap.h"

struct V2MP_VirtualMachine
{
	V2MP_Mainboard* mainboard;
	V2MP_Supervisor* supervisor;
};

V2MP_VirtualMachine* V2MP_VirtualMachine_AllocateAndInit(void)
{
	V2MP_VirtualMachine* vm = V2MP_CALLOC_STRUCT(V2MP_VirtualMachine);

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

	V2MP_FREE(vm);
}

struct V2MP_Mainboard* V2MP_VirtualMachine_GetMainboard(V2MP_VirtualMachine* vm)
{
	return vm ? vm->mainboard : NULL;
}

struct V2MP_Supervisor* V2MP_VirtualMachine_GetSupervisor(V2MP_VirtualMachine* vm)
{
	return vm ? vm->supervisor : NULL;
}

size_t V2MP_VirtualMachile_GetTotalMemoryBytes(V2MP_VirtualMachine* vm)
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
