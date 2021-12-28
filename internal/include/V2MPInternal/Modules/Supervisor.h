#ifndef V2MP_MODULES_SUPERVISOR_H
#define V2MP_MODULES_SUPERVISOR_H

#include <stdbool.h>
#include <stddef.h>
#include "V2MPInternal/Defs.h"

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
	size_t dsLengthInWords
);

void V2MP_Supervisor_ClearProgram(V2MP_Supervisor* supervisor);
bool V2MP_Supervisor_IsProgramLoaded(const V2MP_Supervisor* supervisor);

bool V2MP_Supervisor_ExecuteClockCycle(V2MP_Supervisor* supervisor);

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_SUPERVISOR_H
