#include "V2MPInternal/Modules/Mainboard.h"
#include "V2MPInternal/Modules/CPU.h"
#include "V2MPInternal/Modules/MemoryStore.h"
#include "V2MPInternal/Util/Heap.h"

struct V2MP_Mainboard
{
	V2MP_CPURenameMe* cpu;
	V2MP_MemoryStoreRenameMe* memoryStore;
};

static inline bool HasAllModules(V2MP_Mainboard* board)
{
	return
		board &&
		board->cpu &&
		board->memoryStore;
}

V2MP_Mainboard* V2MP_Mainboard_AllocateAndInit(void)
{
	V2MP_Mainboard* board = V2MP_CALLOC_STRUCT(V2MP_Mainboard);

	if ( !board )
	{
		return NULL;
	}

	board->memoryStore = V2MP_MemoryStoreRenameMe_AllocateAndInit();
	board->cpu = V2MP_CPURenameMe_AllocateAndInit();

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
		V2MP_CPURenameMe_DeinitAndFree(board->cpu);
		board->cpu = NULL;
	}

	if ( board->memoryStore )
	{
		V2MP_MemoryStoreRenameMe_DeinitAndFree(board->memoryStore);
		board->memoryStore = NULL;
	}

	V2MP_FREE(board);
}

struct V2MP_CPURenameMe* V2MP_Mainboard_GetCPU(const V2MP_Mainboard* board)
{
	return board ? board->cpu : NULL;
}

struct V2MP_MemoryStoreRenameMe* V2MP_Mainboard_GetMemoryStore(const V2MP_Mainboard* board)
{
	return board ? board->memoryStore : NULL;
}
