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
	V2MP_DoubleLL* newActions;
	V2MP_DoubleLL* ongoingActions;

	MemorySegment programCS;
	MemorySegment programDS;

	V2MP_Mainboard* mainboard;
};

static inline void ResetProgramMemorySegment(MemorySegment* seg)
{
	seg->base = 0;
	seg->lengthInBytes = 0;
}

static inline bool DataRangeIsInSegment(const MemorySegment* seg, size_t address, size_t numBytes)
{
	// Check is specially constructed to avoid possibility of a size_t overflow.
	return
		seg &&
		address < seg->lengthInBytes &&
		numBytes <= seg->lengthInBytes &&
		address <= seg->lengthInBytes - numBytes;
}

V2MP_Byte* V2MP_Supervisor_GetDataRangeFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	size_t numBytes
);

const V2MP_Byte* V2MP_Supervisor_GetConstDataRangeFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	size_t numBytes
);

bool V2MP_Supervisor_FetchWordFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	V2MP_Word* outWord
);

bool V2MP_Supervisor_ReadRangeFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	V2MP_Byte* outBuffer,
	size_t numBytes
);

void V2MP_Supervisor_SetCPUFault(V2MP_Supervisor* supervisor, V2MP_Word fault);
size_t V2MP_Supervisor_GetMaxDSBytesAvailableAtAddress(V2MP_Supervisor* supervisor, V2MP_Word address);

V2MP_Word V2MP_Supervisor_FetchInstructionWord(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word* destReg);
void V2MP_Supervisor_RequestLoadWordFromDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_RegisterIndex destReg);
void V2MP_Supervisor_RequestStoreWordToDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word wordToStore);

void V2MP_Supervisor_PerformDevicePortQuery(V2MP_Supervisor* supervisor, V2MP_Word port, V2MP_Word queryType);

void V2MP_Supervisor_RequestDevicePortDirectRead(
	V2MP_Supervisor* supervisor,
	V2MP_Word port
);

void V2MP_Supervisor_RequestDevicePortIndirectRead(
	V2MP_Supervisor* supervisor,
	V2MP_Word port,
	V2MP_Word dsSrcAddress,
	V2MP_Word dsMaxBytes
);

void V2MP_Supervisor_RequestDevicePortDirectWrite(
	V2MP_Supervisor* supervisor,
	V2MP_Word port
);

void V2MP_Supervisor_RequestDevicePortIndirectWrite(
	V2MP_Supervisor* supervisor,
	V2MP_Word port,
	V2MP_Word dsDestAddress,
	V2MP_Word dsMaxBytes
);

void V2MP_Supervisor_RequestRelinquishMailbox(V2MP_Supervisor* supervisor, V2MP_Word port);
void V2MP_Supervisor_RequestUsableByteCount(V2MP_Supervisor* supervisor, V2MP_Word port);

#endif // V2MP_MODULES_SUPERVISOR_INTERNAL_H
