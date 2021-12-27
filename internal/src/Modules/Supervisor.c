#include "V2MPInternal/Modules/Supervisor.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Util/Heap.h"
#include "Modules/Supervisor_Internal.h"
#include "Modules/Supervisor_CPUInterface.h"

static void DetachFromMainboard(V2MP_Supervisor* supervisor)
{
	V2MP_CPURenameMe* cpu;

	if ( !supervisor || !supervisor->mainboard )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPURenameMe_ResetSupervisorInterface(cpu);
	}
}

static void AttachToMainboard(V2MP_Supervisor* supervisor)
{
	V2MP_CPURenameMe* cpu;

	if ( !supervisor || !supervisor->mainboard )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPURenameMe_SupervisorInterface interface;
		V2MP_Supervisor_CreateCPUInterface(supervisor, &interface);
		V2MP_CPURenameMe_SetSupervisorInterface(cpu, &interface);
	}
}

V2MP_Supervisor* V2MP_Supervisor_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_Supervisor);
}

void V2MP_Supervisor_DeinitAndFree(V2MP_Supervisor* supervisor)
{
	if ( !supervisor )
	{
		return;
	}

	V2MP_Supervisor_SetMainboard(supervisor, NULL);

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

bool V2MP_Supervisor_ExecuteClockCycle(V2MP_Supervisor* supervisor)
{
	V2MP_CPURenameMe* cpu;

	if ( !supervisor || !supervisor->mainboard )
	{
		return false;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu || !V2MP_CPURenameMe_ExecuteClockCycle(cpu) )
	{
		return false;
	}

	// TODO:
	// - Check for pending requests as a result of the instruction
	// - Resolve requests as appropriate
	return false;
}
