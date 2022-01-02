#include <string.h>
#include "Modules/Supervisor_Internal.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/Mainboard.h"

V2MP_Byte* V2MP_Supervisor_GetDataRangeFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	size_t numBytes
)
{
	V2MP_MemoryStore* memoryStore;

	if ( !DataRangeIsInSegment(seg, address, numBytes) )
	{
		return NULL;
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return NULL;
	}

	return V2MP_MemoryStore_GetPtrToRange(memoryStore, seg->base + address, numBytes);
}

const V2MP_Byte* V2MP_Supervisor_GetConstDataRangeFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	size_t numBytes
)
{
	V2MP_MemoryStore* memoryStore;

	if ( !DataRangeIsInSegment(seg, address, numBytes) )
	{
		return NULL;
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return NULL;
	}

	return V2MP_MemoryStore_GetConstPtrToRange(memoryStore, seg->base + address, numBytes);
}

bool V2MP_Supervisor_FetchWordFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	V2MP_Word* outWord
)
{
	V2MP_MemoryStore* memoryStore;

	if ( !supervisor || !DataRangeIsInSegment(seg, address, sizeof(V2MP_Word)) )
	{
		return false;
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return false;
	}

	return V2MP_MemoryStore_LoadWord(memoryStore, seg->base + address, outWord);
}

bool V2MP_Supervisor_ReadRangeFromSegment(
	const V2MP_Supervisor* supervisor,
	const MemorySegment* seg,
	size_t address,
	V2MP_Byte* outBuffer,
	size_t numBytes
)
{
	const V2MP_Byte* rawData;

	rawData = V2MP_Supervisor_GetConstDataRangeFromSegment(supervisor, seg, address, numBytes);

	if ( !rawData )
	{
		return false;
	}

	memcpy(outBuffer, rawData, numBytes);
	return true;
}

void V2MP_Supervisor_SetCPUFault(V2MP_Supervisor* supervisor, V2MP_Word fault)
{
	V2MP_CPU* cpu;

	if ( !supervisor )
	{
		return;
	}

	cpu = V2MP_Mainboard_GetCPU(supervisor->mainboard);

	if ( cpu )
	{
		V2MP_CPU_NotifyFault(cpu, fault);
	}
}

size_t V2MP_Supervisor_GetMaxDSBytesAvailableAtAddress(V2MP_Supervisor* supervisor, V2MP_Word address)
{
	if ( !supervisor )
	{
		return 0;
	}

	if ( address >= supervisor->programDS.lengthInBytes )
	{
		return 0;
	}

	return supervisor->programDS.lengthInBytes - address;
}

V2MP_Word V2MP_Supervisor_FetchInstructionWord(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word* destReg)
{
	V2MP_MemoryStore* memoryStore;

	if ( !supervisor )
	{
		return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0);
	}

	memoryStore = V2MP_Mainboard_GetMemoryStore(supervisor->mainboard);

	if ( !memoryStore )
	{
		return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, 0);
	}

	if ( !V2MP_MemoryStore_LoadWord(memoryStore, address, destReg) )
	{
		return V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SEG, 0);
	}

	return V2MP_FAULT_NONE;
}

void V2MP_Supervisor_RequestLoadWordFromDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_RegisterIndex destReg)
{
	V2MP_Supervisor_Action* action;

	action = V2MP_Supervisor_CreateNewAction(supervisor);

	if ( !action )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, SVAT_LOAD_WORD));
		return;
	}

	action->actionType = SVAT_LOAD_WORD;
	SVACTION_LOAD_WORD_ARG_ADDRESS(action) = address;
	SVACTION_LOAD_WORD_ARG_DESTREG(action) = destReg;
}

void V2MP_Supervisor_RequestStoreWordToDS(V2MP_Supervisor* supervisor, V2MP_Word address, V2MP_Word wordToStore)
{
	V2MP_Supervisor_Action* action;

	action = V2MP_Supervisor_CreateNewAction(supervisor);

	if ( !action )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, SVAT_STORE_WORD));
		return;
	}

	action->actionType = SVAT_STORE_WORD;
	SVACTION_STORE_WORD_ARG_ADDRESS(action) = address;
	SVACTION_STORE_WORD_ARG_WORD(action) = wordToStore;
}

void V2MP_Supervisor_RequestDevicePortIndirectRead(
	V2MP_Supervisor* supervisor,
	V2MP_Word port,
	V2MP_Word dsDestAddress,
	V2MP_Word dsMaxBytes
)
{
	V2MP_Supervisor_Action* action;

	action = V2MP_Supervisor_CreateNewAction(supervisor);

	if ( !action )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, SVAT_STORE_WORD));
		return;
	}

	action->actionType = SVAT_DEVICE_DATA_TRANSFER;
	SVACTION_DDT_ARG_PORT(action) = port;
	SVACTION_DDT_ARG_DS_ADDR(action) = dsDestAddress;
	SVACTION_DDT_ARG_DS_SIZE(action) = dsMaxBytes;
	SVACTION_DDT_ARG_FLAGS(action) = 0;
}

void V2MP_Supervisor_RequestDevicePortIndirectWrite(
	V2MP_Supervisor* supervisor,
	V2MP_Word port,
	V2MP_Word dsSrcAddress,
	V2MP_Word dsMaxBytes
)
{
	V2MP_Supervisor_Action* action;

	action = V2MP_Supervisor_CreateNewAction(supervisor);

	if ( !action )
	{
		V2MP_Supervisor_SetCPUFault(supervisor, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_SPV, SVAT_STORE_WORD));
		return;
	}

	action->actionType = SVAT_DEVICE_DATA_TRANSFER;
	SVACTION_DDT_ARG_PORT(action) = port;
	SVACTION_DDT_ARG_DS_ADDR(action) = dsSrcAddress;
	SVACTION_DDT_ARG_DS_SIZE(action) = dsMaxBytes;
	SVACTION_DDT_ARG_FLAGS(action) = SVACTION_DDT_FLAG_IS_MB_WRITE;
}
