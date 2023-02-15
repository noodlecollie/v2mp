#include <string.h>
#include "V2MPInternal/Modules/Supervisor.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "LibBaseUtil/Heap.h"
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

static bool HandlePostInstructionTasks(V2MP_Supervisor* supervisor)
{
	if ( !V2MP_Supervisor_ResolveOutstandingActions(supervisor) )
	{
		return false;
	}

	// TODO: Handle fault if one was set?
	return true;
}

V2MP_Supervisor* V2MP_Supervisor_AllocateAndInit(void)
{
	V2MP_Supervisor* supervisor = BASEUTIL_CALLOC_STRUCT(V2MP_Supervisor);

	if ( !supervisor )
	{
		return NULL;
	}

	if ( !V2MP_Supervisor_CreateActionLists(supervisor) )
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
	V2MP_Supervisor_DestroyActionLists(supervisor);

	BASEUTIL_FREE(supervisor);
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
	size_t dsLengthInWords,
	size_t ssLengthInWords
)
{
	V2MP_MemoryStore* memoryStore;
	size_t totalMemoryAvailableInWords;
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

	totalMemoryAvailableInWords = V2MP_MemoryStore_GetTotalMemorySize(memoryStore) / sizeof(V2MP_Word);

	// We do this carefully, to avoid overflowing any size_t calculations:

	// First of all, is either segment on its own larger than the total RAM that we have?
	if ( csLengthInWords > totalMemoryAvailableInWords ||
	     dsLengthInWords > totalMemoryAvailableInWords ||
		 ssLengthInWords > totalMemoryAvailableInWords )
	{
		return false;
	}

	// Next, is any pair larger than the max available memory?
	if ( csLengthInWords + dsLengthInWords > totalMemoryAvailableInWords ||
	     csLengthInWords + ssLengthInWords > totalMemoryAvailableInWords ||
	     dsLengthInWords + ssLengthInWords > totalMemoryAvailableInWords )
	{
		return false;
	}

	// Finally, is the entire sum larger than the max available memory?
	if ( csLengthInWords + dsLengthInWords + ssLengthInWords > totalMemoryAvailableInWords )
	{
		return false;
	}

	supervisor->programCS.base = 0;
	supervisor->programCS.lengthInBytes = csLengthInWords * sizeof(V2MP_Word);

	supervisor->programDS.base = supervisor->programCS.base + supervisor->programCS.lengthInBytes;
	supervisor->programDS.lengthInBytes = dsLengthInWords * sizeof(V2MP_Word);

	supervisor->programSS.base = supervisor->programDS.base + supervisor->programDS.lengthInBytes;
	supervisor->programSS.lengthInBytes = ssLengthInWords * sizeof(V2MP_Word);

	memcpy(rawMemory + supervisor->programCS.base, cs, supervisor->programCS.lengthInBytes);

	if ( supervisor->programDS.lengthInBytes > 0 )
	{
		memcpy(rawMemory + supervisor->programDS.base, ds, supervisor->programDS.lengthInBytes);
	}

	supervisor->programHasExited = false;
	supervisor->programExitCode = 0;

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

bool V2MP_Supervisor_HasProgramExited(const V2MP_Supervisor* supervisor)
{
	return supervisor ? supervisor->programHasExited : false;
}

V2MP_Word V2MP_Supervisor_ProgramExitCode(const V2MP_Supervisor* supervisor)
{
	return supervisor ? supervisor->programExitCode : 0;
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

	return HandlePostInstructionTasks(supervisor);
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

	return HandlePostInstructionTasks(supervisor);
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

bool V2MP_Supervisor_ReadSSRange(
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
		&supervisor->programSS,
		address,
		outBuffer,
		numBytes
	);
}
