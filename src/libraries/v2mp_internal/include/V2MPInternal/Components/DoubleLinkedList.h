#ifndef V2MP_COMPONENTS_DOUBLELINKEDLIST_H
#define V2MP_COMPONENTS_DOUBLELINKEDLIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_DoubleLL V2MP_DoubleLL;
typedef struct V2MP_DoubleLL_Node V2MP_DoubleLL_Node;

// Signature for callback that is called when a linked list node
// is about to be destroyed. After this the payload pointer is
// freed internally - it should NOT be freed by the callback.
// However, if the user of the linked list uses the payload structure
// to store any dynamic data, this data should be freed by the
// callback.
typedef void (*V2MP_DoubleLL_OnDestroyPayloadCallback)(void* payload);

V2MP_DoubleLL* V2MP_DoubleLL_AllocateAndInit(size_t payloadSize, V2MP_DoubleLL_OnDestroyPayloadCallback onDestroyPayloadCallback);
void V2MP_DoubleLL_DeinitAndFree(V2MP_DoubleLL* list);

V2MP_DoubleLL_Node* V2MP_DoubleLL_GetHead(const V2MP_DoubleLL* list);
V2MP_DoubleLL_Node* V2MP_DoubleLL_GetTail(const V2MP_DoubleLL* list);
size_t V2MP_DoubleLL_GetNodeCount(const V2MP_DoubleLL* list);

V2MP_DoubleLL_Node* V2MP_DoubleLL_PrependToHead(V2MP_DoubleLL* list);
V2MP_DoubleLL_Node* V2MP_DoubleLL_AppendToTail(V2MP_DoubleLL* list);
V2MP_DoubleLL_Node* V2MP_DoubleLL_CreateBefore(V2MP_DoubleLL* list, V2MP_DoubleLL_Node* node);
V2MP_DoubleLL_Node* V2MP_DoubleLL_CreateAfter(V2MP_DoubleLL* list, V2MP_DoubleLL_Node* node);
void V2MP_DoubleLL_Clear(V2MP_DoubleLL* list);

void* V2MP_DoubleLLNode_GetPayload(const V2MP_DoubleLL_Node* node);
V2MP_DoubleLL* V2MP_DoubleLLNode_GetOwnerList(const V2MP_DoubleLL_Node* node);
V2MP_DoubleLL_Node* V2MP_DoubleLLNode_GetNext(const V2MP_DoubleLL_Node* node);
V2MP_DoubleLL_Node* V2MP_DoubleLLNode_GetPrev(const V2MP_DoubleLL_Node* node);
void V2MP_DoubleLLNode_Destroy(V2MP_DoubleLL_Node* node);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_COMPONENTS_DOUBLELINKEDLIST_H
