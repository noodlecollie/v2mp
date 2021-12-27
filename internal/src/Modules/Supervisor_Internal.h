#ifndef V2MP_MODULES_SUPERVISOR_INTERNAL_H
#define V2MP_MODULES_SUPERVISOR_INTERNAL_H

#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/Supervisor.h"
#include "V2MPInternal/Modules/Mainboard.h"
#include "Modules/Supervisor_Action.h"
#include "V2MPInternal/Components/DoubleLinkedList.h"

typedef struct MemorySegment
{
	size_t base;
	size_t lengthInBytes;
} MemorySegment;

struct V2MP_Supervisor
{
	V2MP_DoubleLL* actions;
	MemorySegment programCS;
	MemorySegment programDS;

	V2MP_Mainboard* mainboard;
};

static inline void ResetProgramMemorySegment(MemorySegment* seg)
{
	seg->base = 0;
	seg->lengthInBytes = 0;
}

void V2MP_Supervisor_SetCPUFault(V2MP_Supervisor* supervisor, V2MP_Word fault);

V2MP_Word V2MP_Supervisor_FetchInstructionWord(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word* destReg);
void V2MP_Supervisor_RequestLoadWordFromDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_RegisterIndex destReg);
void V2MP_Supervisor_RequestStoreWordToDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word wordToStore);

#endif // V2MP_MODULES_SUPERVISOR_INTERNAL_H
