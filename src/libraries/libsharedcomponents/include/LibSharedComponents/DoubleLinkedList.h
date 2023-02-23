#ifndef SHAREDCOMPONENTS_DOUBLELINKEDLIST_H
#define SHAREDCOMPONENTS_DOUBLELINKEDLIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MPSC_DoubleLL V2MPSC_DoubleLL;
typedef struct V2MPSC_DoubleLL_Node V2MPSC_DoubleLL_Node;

// Signature for callback that is called when a linked list node
// is about to be destroyed. After this the payload pointer is
// freed internally - it should NOT be freed by the callback.
// However, if the user of the linked list uses the payload structure
// to store any dynamic data, this data should be freed by the
// callback.
typedef void (*V2MPSC_DoubleLL_OnDestroyPayloadCallback)(void* payload);

V2MPSC_DoubleLL* V2MPSC_DoubleLL_AllocateAndInit(size_t payloadSize, V2MPSC_DoubleLL_OnDestroyPayloadCallback onDestroyPayloadCallback);
void V2MPSC_DoubleLL_DeinitAndFree(V2MPSC_DoubleLL* list);

V2MPSC_DoubleLL_Node* V2MPSC_DoubleLL_GetHead(const V2MPSC_DoubleLL* list);
V2MPSC_DoubleLL_Node* V2MPSC_DoubleLL_GetTail(const V2MPSC_DoubleLL* list);
size_t V2MPSC_DoubleLL_GetNodeCount(const V2MPSC_DoubleLL* list);

V2MPSC_DoubleLL_Node* V2MPSC_DoubleLL_PrependToHead(V2MPSC_DoubleLL* list);
V2MPSC_DoubleLL_Node* V2MPSC_DoubleLL_AppendToTail(V2MPSC_DoubleLL* list);
V2MPSC_DoubleLL_Node* V2MPSC_DoubleLL_CreateBefore(V2MPSC_DoubleLL* list, V2MPSC_DoubleLL_Node* node);
V2MPSC_DoubleLL_Node* V2MPSC_DoubleLL_CreateAfter(V2MPSC_DoubleLL* list, V2MPSC_DoubleLL_Node* node);
void V2MPSC_DoubleLL_Clear(V2MPSC_DoubleLL* list);

void* V2MPSC_DoubleLLNode_GetPayload(const V2MPSC_DoubleLL_Node* node);
V2MPSC_DoubleLL* V2MPSC_DoubleLLNode_GetOwnerList(const V2MPSC_DoubleLL_Node* node);
V2MPSC_DoubleLL_Node* V2MPSC_DoubleLLNode_GetNext(const V2MPSC_DoubleLL_Node* node);
V2MPSC_DoubleLL_Node* V2MPSC_DoubleLLNode_GetPrev(const V2MPSC_DoubleLL_Node* node);
void V2MPSC_DoubleLLNode_Destroy(V2MPSC_DoubleLL_Node* node);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SHAREDCOMPONENTS_DOUBLELINKEDLIST_H
