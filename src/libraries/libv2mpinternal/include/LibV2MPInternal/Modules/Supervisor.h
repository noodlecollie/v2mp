#ifndef V2MPINTERNAL_MODULES_SUPERVISOR_H
#define V2MPINTERNAL_MODULES_SUPERVISOR_H

#include <stdbool.h>
#include <stddef.h>
#include "LibV2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_Supervisor V2MP_Supervisor;
struct V2MP_Mainboard;

V2MP_Supervisor* V2MP_Supervisor_AllocateAndInit(void);
void V2MP_Supervisor_DeinitAndFree(V2MP_Supervisor* supervisor);

struct V2MP_Mainboard* V2MP_Supervisor_GetMainboard(const V2MP_Supervisor* supervisor);
void V2MP_Supervisor_SetMainboard(V2MP_Supervisor* supervisor, struct V2MP_Mainboard* mainboard);

bool V2MP_Supervisor_LoadProgram(
	V2MP_Supervisor* supervisor,
	const V2MP_Word* cs,
	size_t csLengthInWords,
	const V2MP_Word* ds,
	size_t dsLengthInWords,
	size_t ssLengthInWords
);

void V2MP_Supervisor_ClearProgram(V2MP_Supervisor* supervisor);
bool V2MP_Supervisor_IsProgramLoaded(const V2MP_Supervisor* supervisor);
bool V2MP_Supervisor_HasProgramExited(const V2MP_Supervisor* supervisor);
V2MP_Word V2MP_Supervisor_ProgramExitCode(const V2MP_Supervisor* supervisor);

bool V2MP_Supervisor_ExecuteClockCycle(V2MP_Supervisor* supervisor);
bool V2MP_Supervisor_ExecuteSingleInstruction(V2MP_Supervisor* supervisor, V2MP_Word instruction);

bool V2MP_Supervisor_FetchCSWord(
	const V2MP_Supervisor* supervisor,
	V2MP_Word address,
	V2MP_Word* outWord
);

bool V2MP_Supervisor_FetchDSWord(
	const V2MP_Supervisor* supervisor,
	V2MP_Word address,
	V2MP_Word* outWord
);

bool V2MP_Supervisor_ReadDSRange(
	const V2MP_Supervisor* supervisor,
	V2MP_Word address,
	V2MP_Byte* outBuffer,
	size_t numBytes
);

bool V2MP_Supervisor_ReadSSRange(
	const V2MP_Supervisor* supervisor,
	V2MP_Word address,
	V2MP_Byte* outBuffer,
	size_t numBytes
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPINTERNAL_MODULES_SUPERVISOR_H
