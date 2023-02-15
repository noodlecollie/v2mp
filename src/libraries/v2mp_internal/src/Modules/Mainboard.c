#include "V2MPInternal/Modules/Mainboard.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "LibBaseUtil/Heap.h"

struct V2MP_Mainboard
{
	V2MP_CPU* cpu;
	V2MP_MemoryStore* memoryStore;
};

static inline bool HasAllModules(V2MP_Mainboard* board)
{
	return
		board &&
		board->memoryStore &&
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
