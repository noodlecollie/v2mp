#include <string.h>
#include "Modules/Supervisor_Action_Stack.h"
#include "Modules/Supervisor_Internal.h"
#include "LibV2MPInternal/Modules/Mainboard.h"
#include "LibV2MPInternal/Modules/CPU.h"

bool V2MP_Supervisor_PerformStackPush(V2MP_Supervisor* supervisor, const V2MP_Word* inWords, size_t numWords)
{
	V2MP_CPU* cpu;
	V2MP_Byte* stackData;
	V2MP_Word sp;

	if ( !supervisor || !inWords || numWords < 1 )
	{
		return false;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu )
	{
		return false;
	}

	sp = V2MP_CPU_GetStackPointer(cpu);

	stackData = V2MP_Supervisor_GetDataRangeFromSegment(
		supervisor,
		&supervisor->programSS,
		sp,
		numWords * sizeof(V2MP_Word)
	);

	if ( !stackData )
	{
		return false;
	}

	memcpy(stackData, inWords, numWords * sizeof(V2MP_Word));
	sp += (V2MP_Word)(numWords * sizeof(V2MP_Word));

	V2MP_CPU_SetStackPointer(cpu, sp);

	return true;
}

bool V2MP_Supervisor_PerformStackPop(V2MP_Supervisor* supervisor, V2MP_Word* outWords, size_t numWords)
{
	V2MP_CPU* cpu;
	const V2MP_Byte* stackData;
	V2MP_Word sp;

	if ( !supervisor )
	{
		return false;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( !cpu )
	{
		return false;
	}

	sp = V2MP_CPU_GetStackPointer(cpu);

	if ( sp < numWords * sizeof(V2MP_Word) )
	{
		return false;
	}

	sp -= (V2MP_Word)(numWords * sizeof(V2MP_Word));

	stackData = V2MP_Supervisor_GetDataRangeFromSegment(
		supervisor,
		&supervisor->programSS,
		sp,
		numWords * sizeof(V2MP_Word)
	);

	if ( !stackData )
	{
		// Should never happen.
		return false;
	}

	memcpy(outWords, stackData, numWords * sizeof(V2MP_Word));
	V2MP_CPU_SetStackPointer(cpu, sp);

	return true;
}
