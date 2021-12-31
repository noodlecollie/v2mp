#include "V2MPInternal/Components/DoubleLinkedList.h"
#include "V2MPInternal/Util/Heap.h"

struct V2MP_DoubleLL_Node
{
	V2MP_DoubleLL* list;
	V2MP_DoubleLL_Node* prev;
	V2MP_DoubleLL_Node* next;
	void* payload;
};

struct V2MP_DoubleLL
{
	V2MP_DoubleLL_Node* head;
	V2MP_DoubleLL_Node* tail;
	size_t numNodes;

	size_t payloadSize;
	V2MP_DoubleLL_OnDestroyPayloadCallback onDestroyPayloadCallback;
};

static inline V2MP_DoubleLL_Node* AllocateNodeAndPayload(V2MP_DoubleLL* list)
{
	V2MP_DoubleLL_Node* node = V2MP_CALLOC_STRUCT(V2MP_DoubleLL_Node);

	if ( !node )
	{
		return NULL;
	}

	node->payload = V2MP_MALLOC(list->payloadSize);

	if ( !node->payload )
	{
		V2MP_FREE(node);
		node = NULL;
	}

	node->list = list;
	return node;
}

static inline void FreeNode(V2MP_DoubleLL_Node* node, V2MP_DoubleLL_OnDestroyPayloadCallback onDestroyPayloadCallback)
{
	if ( onDestroyPayloadCallback )
	{
		onDestroyPayloadCallback(node->payload);
		V2MP_FREE(node->payload);
	}

	V2MP_FREE(node);
}

static void FreeAllNodes(V2MP_DoubleLL* list)
{
	V2MP_DoubleLL_Node* node = list->head;

	while ( node )
	{
		V2MP_DoubleLL_Node* next = node->next;
		FreeNode(node, list->onDestroyPayloadCallback);
		node = next;
	}

	list->head = NULL;
	list->tail = NULL;
	list->numNodes = 0;
}

V2MP_DoubleLL* V2MP_DoubleLL_AllocateAndInit(size_t payloadSize, V2MP_DoubleLL_OnDestroyPayloadCallback onDestroyPayloadCallback)
{
	if ( payloadSize < 1 )
	{
		return NULL;
	}

	V2MP_DoubleLL* list = V2MP_CALLOC_STRUCT(V2MP_DoubleLL);

	if ( !list )
	{
		return NULL;
	}

	list->payloadSize = payloadSize;
	list->onDestroyPayloadCallback = onDestroyPayloadCallback;

	return list;
}

void V2MP_DoubleLL_DeinitAndFree(V2MP_DoubleLL* list)
{
	if ( !list )
	{
		return;
	}

	FreeAllNodes(list);
	V2MP_FREE(list);
}

V2MP_DoubleLL_Node* V2MP_DoubleLL_GetHead(const V2MP_DoubleLL* list)
{
	return list ? list->head : NULL;
}

V2MP_DoubleLL_Node* V2MP_DoubleLL_GetTail(const V2MP_DoubleLL* list)
{
	return list ? list->tail : NULL;
}

size_t V2MP_DoubleLL_GetNodeCount(const V2MP_DoubleLL* list)
{
	return list ? list->numNodes : 0;
}

V2MP_DoubleLL_Node* V2MP_DoubleLL_PrependToHead(V2MP_DoubleLL* list)
{
	V2MP_DoubleLL_Node* node;

	if ( !list )
	{
		return NULL;
	}

	node = AllocateNodeAndPayload(list);

	if ( !node )
	{
		return NULL;
	}

	if ( list->head )
	{
		list->head->prev = node;
		node->next = list->head;
	}

	list->head = node;

	if ( !list->tail )
	{
		list->tail = node;
	}

	++list->numNodes;

	return node;
}

V2MP_DoubleLL_Node* V2MP_DoubleLL_AppendToTail(V2MP_DoubleLL* list)
{
	V2MP_DoubleLL_Node* node;

	if ( !list )
	{
		return NULL;
	}

	node = AllocateNodeAndPayload(list);

	if ( !node )
	{
		return NULL;
	}

	if ( list->tail )
	{
		list->tail->next = node;
		node->prev = list->tail;
	}

	list->tail = node;

	if ( !list->head )
	{
		list->head = node;
	}

	++list->numNodes;

	return node;
}

V2MP_DoubleLL_Node* V2MP_DoubleLL_CreateBefore(V2MP_DoubleLL* list, V2MP_DoubleLL_Node* node)
{
	V2MP_DoubleLL_Node* newNode;

	if ( !list || !node || node->list != list )
	{
		return NULL;
	}

	if ( node == list->head )
	{
		return V2MP_DoubleLL_PrependToHead(list);
	}

	newNode = AllocateNodeAndPayload(list);

	if ( !newNode )
	{
		return NULL;
	}

	newNode->prev = node->prev;
	newNode->next = node;

	node->prev->next = newNode;
	node->prev = newNode;

	++list->numNodes;

	return newNode;
}

V2MP_DoubleLL_Node* V2MP_DoubleLL_CreateAfter(V2MP_DoubleLL* list, V2MP_DoubleLL_Node* node)
{
	V2MP_DoubleLL_Node* newNode;

	if ( !list || !node || node->list != list )
	{
		return NULL;
	}

	if ( node == list->tail )
	{
		return V2MP_DoubleLL_AppendToTail(list);
	}

	newNode = AllocateNodeAndPayload(list);

	if ( !newNode )
	{
		return NULL;
	}

	newNode->prev = node;
	newNode->next = node->next;

	node->next->prev = newNode;
	node->next = newNode;

	++list->numNodes;

	return newNode;
}

void V2MP_DoubleLL_Clear(V2MP_DoubleLL* list)
{
	if ( !list )
	{
		return;
	}

	FreeAllNodes(list);
}

void* V2MP_DoubleLLNode_GetPayload(const V2MP_DoubleLL_Node* node)
{
	return node ? node->payload : NULL;
}

V2MP_DoubleLL* V2MP_DoubleLLNode_GetOwnerList(const V2MP_DoubleLL_Node* node)
{
	return node ? node->list : NULL;
}

V2MP_DoubleLL_Node* V2MP_DoubleLLNode_GetNext(const V2MP_DoubleLL_Node* node)
{
	return node ? node->next : NULL;
}

V2MP_DoubleLL_Node* V2MP_DoubleLLNode_GetPrev(const V2MP_DoubleLL_Node* node)
{
	return node ? node->prev : NULL;
}

void V2MP_DoubleLLNode_Destroy(V2MP_DoubleLL_Node* node)
{
	V2MP_DoubleLL* list;

	if ( !node )
	{
		return;
	}

	// If the node has no list, something has gone seriously wrong,
	// as the only place that can be modified is within this file.
	// As a consequence, we don't check the validity of the list
	// pointer as we consider a null list pointer a programmer error.
	list = node->list;

	if ( node == list->head )
	{
		list->head = node->next;
	}

	if ( node == list->tail )
	{
		list->tail = node->prev;
	}

	if ( node->prev )
	{
		node->prev->next = node->next;
	}

	if ( node->next )
	{
		node->next->prev = node->prev;
	}

	FreeNode(node, list->onDestroyPayloadCallback);
	--list->numNodes;
}
