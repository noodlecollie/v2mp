#include "V2MPInternal/Modules/Mainboard.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DeviceCollection.h"
#include "BaseUtil/Heap.h"

struct V2MP_Mainboard
{
	V2MP_CPU* cpu;
	V2MP_MemoryStore* memoryStore;
	V2MP_DevicePortCollection* devicePorts;
	V2MP_DeviceCollection* devices;
};

static inline bool HasAllModules(V2MP_Mainboard* board)
{
	return
		board &&
		board->memoryStore &&
		board->devicePorts &&
		board->devices &&
		board->cpu;
}

V2MP_Mainboard* V2MP_Mainboard_AllocateAndInit(void)
{
	V2MP_Mainboard* board = BASEUTIL_CALLOC_STRUCT(V2MP_Mainboard);

	if ( !board )
	{
		return NULL;
	}

	board->memoryStore = V2MP_MemoryStore_AllocateAndInit();
	board->devicePorts = V2MP_DevicePortCollection_AllocateAndInit();
	board->devices = V2MP_DeviceCollection_AllocateAndInit();
	board->cpu = V2MP_CPU_AllocateAndInit();

	if ( !HasAllModules(board) )
	{
		V2MP_Mainboard_DeinitAndFree(board);
		board = NULL;
	}

	return board;
}

void V2MP_Mainboard_DeinitAndFree(V2MP_Mainboard* board)
{
	if ( !board )
	{
		return;
	}

	if ( board->cpu )
	{
		V2MP_CPU_DeinitAndFree(board->cpu);
		board->cpu = NULL;
	}

	if ( board->devicePorts )
	{
		V2MP_DevicePortCollection_DeinitAndFree(board->devicePorts);
		board->devicePorts = NULL;
	}

	if ( board->devices )
	{
		V2MP_DeviceCollection_DeinitAndFree(board->devices);
		board->devices = NULL;
	}

	if ( board->memoryStore )
	{
		V2MP_MemoryStore_DeinitAndFree(board->memoryStore);
		board->memoryStore = NULL;
	}

	BASEUTIL_FREE(board);
}

struct V2MP_CPU* V2MP_Mainboard_GetCPU(const V2MP_Mainboard* board)
{
	return board ? board->cpu : NULL;
}

struct V2MP_MemoryStore* V2MP_Mainboard_GetMemoryStore(const V2MP_Mainboard* board)
{
	return board ? board->memoryStore : NULL;
}

struct V2MP_DevicePortCollection* V2MP_Mainboard_GetDevicePortCollection(const V2MP_Mainboard* board)
{
	return board ? board->devicePorts : NULL;
}

struct V2MP_DeviceCollection* V2MP_Mainboard_GetDeviceCollection(const V2MP_Mainboard* board)
{
	return board ? board->devices : NULL;
}
