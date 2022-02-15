#include <ctype.h>
#include "BaseUtil/Heap.h"
#include "BaseUtil/Util.h"
#include "BaseUtil/UTHash_V2MP.h"
#include "Commands/CommandMeta.h"
#include "Commands/CommandMeta_Add.h"

const V2MPAsm_CommandMeta* CommandList[] =
{
	&V2MPAsm_CommandMeta_Add
};

typedef struct CommandTableItem
{
	const char* commandName;
	const V2MPAsm_CommandMeta* commandMeta;
	UT_hash_handle hh;
} CommandTableItem;

static struct CommandTableItem* CommandTable = NULL;

static void AddToTable(const V2MPAsm_CommandMeta* command)
{
	CommandTableItem* item;

	item = BASEUTIL_MALLOC_STRUCT(CommandTableItem);

	if ( !item )
	{
		return;
	}

	item->commandName = command->name;
	item->commandMeta = command;

	HASH_ADD_STR(CommandTable, commandName, item);
}

void V2MPAsm_CommandMeta_InitTable(void)
{
	size_t index;

	V2MPAsm_CommandMeta_DestroyTable();

	for ( index = 0; index < BASEUTIL_ARRAY_SIZE(CommandList); ++index )
	{
		if ( CommandList[index] )
		{
			AddToTable(CommandList[index]);
		}
	}
}

void V2MPAsm_CommandMeta_DestroyTable(void)
{
	HASH_CLEAR(hh, CommandTable);
}

const V2MPAsm_CommandMeta* V2MPAsm_CommandMeta_FindCommand(const char* name, size_t nameLength)
{
	char* buffer;
	size_t index;
	CommandTableItem* item = NULL;

	if ( !CommandTable )
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

	HASH_FIND_STR(CommandTable, buffer, item);

	return item ? item->commandMeta : NULL;
}
