#include <string.h>
#include "V2MPInternal/Modules/Supervisor.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/DeviceCollection.h"
#include "V2MPInternal/Modules/Device.h"
#include "V2MPInternal/Util/Heap.h"
#include "Modules/Supervisor_Internal.h"
#include "Modules/Supervisor_CPUInterface.h"

static void DetachFromMainboard(V2MP_Supervisor* supervisor)
{
	V2MP_CPU* cpu;

	if ( !supervisor || !supervisor->mainboard )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPU_ResetSupervisorInterface(cpu);
	}
}

static void AttachToMainboard(V2MP_Supervisor* supervisor)
{
	V2MP_CPU* cpu;

	if ( !supervisor || !supervisor->mainboard )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPU_SupervisorInterface interface;
		V2MP_Supervisor_CreateCPUInterface(supervisor, &interface);
		V2MP_CPU_SetSupervisorInterface(cpu, &interface);
	}
}

static bool PollAllDevices(V2MP_Supervisor* supervisor)
{
	V2MP_DeviceCollection* devices;
	V2MP_Device* device;

	devices = V2MP_Mainboard_GetDeviceCollection(supervisor->mainboard);

	if ( !devices )
	{
		return false;
	}

	for ( device = V2MP_DeviceCollection_GetFirstDevice(devices);
	      device;
	      device = V2MP_DeviceCollection_GetNext(devices, device) )
	{
		V2MP_Device_Poll(device);
	}

	return true;
}

V2MP_Supervisor* V2MP_Supervisor_AllocateAndInit(void)
{
	V2MP_Supervisor* supervisor = V2MP_CALLOC_STRUCT(V2MP_Supervisor);

	if ( !supervisor )
	{
		return NULL;
	}

	if ( !V2MP_Supervisor_CreateActionList(supervisor) )
	{
		V2MP_Supervisor_DeinitAndFree(supervisor);
		supervisor = NULL;
	}

	return supervisor;
}

void V2MP_Supervisor_DeinitAndFree(V2MP_Supervisor* supervisor)
{
	if ( !supervisor )
	{
		return;
	}

	V2MP_Supervisor_SetMainboard(supervisor, NULL);
	V2MP_Supervisor_DestroyActionList(supervisor);

	V2MP_FREE(supervisor);
}

struct V2MP_Mainboard* V2MP_Supervisor_GetMainboard(const V2MP_Supervisor* supervisor)
{
	return supervisor ? supervisor->mainboard : NULL;
}

void V2MP_Supervisor_SetMainboard(V2MP_Supervisor* supervisor, struct V2MP_Mainboard* mainboard)
{
	if ( !supervisor )
	{
		return;
	}

	DetachFromMainboard(supervisor);
	supervisor->mainboard = mainboard;
	AttachToMainboard(supervisor);
}

bool V2MP_Supervisor_LoadProgram(
	V2MP_Supervisor* supervisor,
	const V2MP_Word* cs,
	size_t csLengthInWords,
	const V2MP_Word* ds,
	size_t dsLengthInWords
)
{
	V2MP_MemoryStore* memoryStore;
	size_t totalMemoryAvailable;
	V2MP_Byte* rawMemory = NULL;

	if ( !supervisor || !cs || csLengthInWords < 1 || (!ds && dsLengthInWords > 0) )
	{
		return false;
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return false;
	}

	rawMemory = V2MP_MemoryStore_GetPtrToBase(memoryStore);

	if ( !rawMemory )
	{
		return false;
	}

	totalMemoryAvailable = V2MP_MemoryStore_GetTotalMemorySize(memoryStore);

	// We do this carefully, to avoid overflowing any size_t calculations:

	// First of all, is either segment on its own larger than the total RAM that we have?
	if ( csLengthInWords > totalMemoryAvailable / sizeof(V2MP_Word) || dsLengthInWords > totalMemoryAvailable / sizeof(V2MP_Word) )
	{
		return false;
	}

	// Now that we know that both the CS and DS lengths are no larger than (in the worst case) half
	// of the maximum value of a size_t, we can add them together and do a final comparison.
	if ( csLengthInWords + dsLengthInWords > totalMemoryAvailable / sizeof(V2MP_Word) )
	{
		return false;
	}

	supervisor->programCS.base = 0;
	supervisor->programCS.lengthInBytes = csLengthInWords * sizeof(V2MP_Word);

	supervisor->programDS.base = supervisor->programCS.base + supervisor->programCS.lengthInBytes;
	supervisor->programDS.lengthInBytes = dsLengthInWords * sizeof(V2MP_Word);

	memcpy(rawMemory + supervisor->programCS.base, cs, supervisor->programCS.lengthInBytes);

	if ( supervisor->programDS.lengthInBytes > 0 )
	{
		memcpy(rawMemory + supervisor->programDS.base, ds, supervisor->programDS.lengthInBytes);
	}

	return true;
}

void V2MP_Supervisor_ClearProgram(V2MP_Supervisor* supervisor)
{
	V2MP_CPU* cpu;

	if ( !supervisor )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPU_Reset(cpu);
	}

	ResetProgramMemorySegment(&supervisor->programCS);
	ResetProgramMemorySegment(&supervisor->programDS);
}

bool V2MP_Supervisor_IsProgramLoaded(const V2MP_Supervisor* supervisor)
{
	return supervisor && supervisor->programCS.lengthInBytes > 0;
}

bool V2MP_Supervisor_ExecuteClockCycle(V2MP_Supervisor* supervisor)
{
	V2MP_CPU* cpu;

	if ( !supervisor || !supervisor->mainboard )
	{
		return false;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu || !V2MP_CPU_ExecuteClockCycle(cpu) )
	{
		return false;
	}

	if ( !V2MP_Supervisor_ResolveOutstandingActions(supervisor) )
	{
		return false;
	}

	// TODO: Handle fault if one was set?
	return true;
}

bool V2MP_Supervisor_ExecuteSingleInstruction(V2MP_Supervisor* supervisor, V2MP_Word instruction)
{
	V2MP_CPU* cpu;

	if ( !supervisor || !supervisor->mainboard )
	{
		return false;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu || !V2MP_CPU_ExecuteSingleInstruction(cpu, instruction) )
	{
		return false;
	}

	if ( !V2MP_Supervisor_ResolveOutstandingActions(supervisor) )
	{
		return false;
	}

	if ( !PollAllDevices(supervisor) )
	{
		return false;
	}

	// TODO: Handle fault if one was set?
	return true;
}

bool V2MP_Supervisor_FetchCSWord(
	const V2MP_Supervisor* supervisor,
	V2MP_Word address,
	V2MP_Word* outWord
)
{
	if ( !supervisor )
	{
		return false;
	}

	return V2MP_Supervisor_FetchWordFromSegment(supervisor, &supervisor->programCS, address, outWord);
}

bool V2MP_Supervisor_FetchDSWord(
	const V2MP_Supervisor* supervisor,
	V2MP_Word address,
	V2MP_Word* outWord
)
{
	if ( !supervisor )
	{
		return false;
	}

	return V2MP_Supervisor_FetchWordFromSegment(supervisor, &supervisor->programDS, address, outWord);
}

bool V2MP_Supervisor_ReadDSRange(
	const V2MP_Supervisor* supervisor,
	V2MP_Word address,
	V2MP_Byte* outBuffer,
	size_t numBytes
)
{
	if ( !supervisor || !outBuffer || numBytes < 1 )
	{
		return false;
	}

	return V2MP_Supervisor_ReadRangeFromSegment(
		supervisor,
		&supervisor->programDS,
		address,
		outBuffer,
		numBytes
	);
}
