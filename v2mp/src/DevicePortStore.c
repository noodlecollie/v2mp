#include <stddef.h>
#include <stdlib.h>
#include "V2MP/DevicePortStore.h"
#include "V2MP/DevicePort.h"
#include "DataStructures/HexTree.h"

// TODO: We might want to make this a generic data structure class somehow.
typedef struct V2MP_DevicePort_ListItem
{
	struct V2MP_DevicePort_ListItem* prev;
	struct V2MP_DevicePort_ListItem* next;

	V2MP_DevicePort* port;
} V2MP_DevicePort_ListItem;

struct V2MP_DevicePortStore
{
	V2MP_DevicePort_ListItem* head;
	V2MP_DevicePort_ListItem* tail;
	V2MP_HexTreeNode tree;
};

static inline V2MP_DevicePort_ListItem* AllocateListItem(void)
{
	return (V2MP_DevicePort_ListItem*)calloc(1, sizeof(V2MP_DevicePort_ListItem));
}

static inline void FreeListItem(V2MP_DevicePort_ListItem* item)
{
	V2MP_DevicePort_DeinitAndFree(item->port);
	free(item);
}

static void AppendToPortList(V2MP_DevicePortStore* store, V2MP_DevicePort_ListItem* item)
{
	if ( !store->head )
	{
		store->head = item;
		store->tail = item;
		item->prev = NULL;
		item->next = NULL;

		return;
	}

	item->prev = store->tail;
	item->next = NULL;
	store->tail->next = item;
	store->tail = item;
}

static void RemoveFromPortList(V2MP_DevicePortStore* store, V2MP_DevicePort_ListItem* item)
{
	if ( item == store->head )
	{
		store->head = item->next;
	}

	if ( item == store->tail )
	{
		store->tail = item->prev;
	}

	if ( item->prev )
	{
		item->prev->next = item->next;
	}

	if ( item->next )
	{
		item->next->prev = item->prev;
	}

	item->prev = NULL;
	item->next = NULL;
}

static void FreePortList(V2MP_DevicePortStore* store)
{
	V2MP_DevicePort_ListItem* item;

	item = store->head;

	while ( item )
	{
		V2MP_DevicePort_ListItem* next = item->next;

		FreeListItem(item);
		item = next;
	}
}

size_t V2MP_DevicePortStore_Footprint(void)
{
	return sizeof(V2MP_DevicePortStore);
}

V2MP_DevicePortStore* V2MP_DevicePortStore_AllocateAndInit(void)
{
	V2MP_DevicePortStore* store;

	store = (V2MP_DevicePortStore*)calloc(1, sizeof(V2MP_DevicePortStore));

	if ( !store )
	{
		return NULL;
	}

	V2MP_HexTree_InitRoot(&store->tree);

	return store;
}

void V2MP_DevicePortStore_DeinitAndFree(V2MP_DevicePortStore* store)
{
	if ( !store )
	{
		return;
	}

	V2MP_HexTree_DeinitRoot(&store->tree);
	FreePortList(store);
	free(store);
}

struct V2MP_DevicePort* V2MP_DevicePortStore_CreatePort(V2MP_DevicePortStore* store, V2MP_Word address)
{
	V2MP_DevicePort_ListItem* item = NULL;

	if ( !store )
	{
		return NULL;
	}

	do
	{
		item = AllocateListItem();

		if ( !item )
		{
			break;
		}

		item->port = V2MP_DevicePort_AllocateAndInit();

		if ( !item->port )
		{
			break;
		}

		if ( !V2MP_HexTree_Insert(&store->tree, address, item) )
		{
			break;
		}

		AppendToPortList(store, item);
		return item->port;
	}
	while ( false );

	if ( item )
	{
		FreeListItem(item);
	}

	return NULL;
}

bool V2MP_DevicePortStore_DestroyPort(V2MP_DevicePortStore* store, V2MP_Word address)
{
	V2MP_DevicePort_ListItem* item;

	if ( !store )
	{
		return false;
	}

	item = V2MP_HexTree_Remove(&store->tree, address);

	if ( !item )
	{
		return false;
	}

	RemoveFromPortList(store, item);
	FreeListItem(item);
	return true;
}

struct V2MP_DevicePort* V2MP_DevicePort_GetPort(V2MP_DevicePortStore* store, V2MP_Word address)
{
	V2MP_DevicePort_ListItem* item;

	if ( !store )
	{
		return NULL;
	}

	item = V2MP_HexTree_Find(&store->tree, address);
	return item ? item->port : NULL;
}
