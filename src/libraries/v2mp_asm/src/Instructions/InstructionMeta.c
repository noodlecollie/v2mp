#include <ctype.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"
#include "BaseUtil/UTHash_V2MP.h"
#include "Instructions/InstructionMeta.h"
#include "Instructions/InstructionMeta_Add.h"

const V2MPAsm_InstructionMeta* InstructionList[] =
{
	&V2MPAsm_InstructionMeta_Add
};

typedef struct CommandTableItem
{
	const char* instructionName;
	const V2MPAsm_InstructionMeta* instructionMeta;
	UT_hash_handle hh;
} CommandTableItem;

static struct CommandTableItem* InstructionTable = NULL;

static void AddToTable(const V2MPAsm_InstructionMeta* command)
{
	CommandTableItem* item;

	item = BASEUTIL_MALLOC_STRUCT(CommandTableItem);

	if ( !item )
	{
		return;
	}

	item->instructionName = command->name;
	item->instructionMeta = command;

	HASH_ADD_STR(InstructionTable, instructionName, item);
}

void V2MPAsm_InstructionMeta_InitTable(void)
{
	size_t index;

	V2MPAsm_InstructionMeta_DestroyTable();

	for ( index = 0; index < BASEUTIL_ARRAY_SIZE(InstructionList); ++index )
	{
		if ( InstructionList[index] )
		{
			AddToTable(InstructionList[index]);
		}
	}
}

void V2MPAsm_InstructionMeta_DestroyTable(void)
{
	HASH_CLEAR(hh, InstructionTable);
}

const V2MPAsm_InstructionMeta* V2MPAsm_InstructionMeta_FindCommand(const char* name, size_t nameLength)
{
	char* buffer;
	size_t index;
	CommandTableItem* item = NULL;

	if ( !InstructionTable )
	{
		// Not set up yet.
		return NULL;
	}

	if ( !name || nameLength < 1 )
	{
		return NULL;
	}

	buffer = (char*)BASEUTIL_MALLOC(nameLength + 1);

	if ( !buffer )
	{
		return NULL;
	}

	for ( index = 0; index < nameLength; ++index )
	{
		buffer[index] = tolower(name[index]);
	}

	buffer[nameLength] = '\0';

	HASH_FIND_STR(InstructionTable, buffer, item);
	BASEUTIL_FREE(buffer);

	return item ? item->instructionMeta : NULL;
}
