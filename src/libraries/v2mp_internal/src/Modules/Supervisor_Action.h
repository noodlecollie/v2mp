#ifndef V2MP_MODULES_SUPERVISOR_ACTION_H
#define V2MP_MODULES_SUPERVISOR_ACTION_H

#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/Supervisor.h"
#include "SharedComponents/DoubleLinkedList.h"

#define V2MP_SUPERVISOR_ACTION_LIST \
	LIST_ITEM(SVAT_LOAD_WORD = 0, V2MP_Supervisor_HandleLoadWord) \
	LIST_ITEM(SVAT_STORE_WORD, V2MP_Supervisor_HandleStoreWord) \
	LIST_ITEM(SVAT_DEVICE_PORT_QUERY, V2MP_Supervisor_HandlePerformDevicePortQuery) \
	LIST_ITEM(SVAT_DEVICE_DATA_TRANSFER, V2MP_Supervisor_HandleDeviceDataTransfer) \
	LIST_ITEM(SVAT_RELINQUISH_PORT_MAILBOX, V2MP_Supervisor_HandleRelinquishPortMailbox) \
	LIST_ITEM(SVAT_PORT_MAILBOX_USABLE_BYTE_COUNT, V2MP_Supervisor_HandleGetUsableByteCount) \
	LIST_ITEM(SVAT_STACK_OPERATION, V2MP_Supervisor_HandleStackOperation)

#define LIST_ITEM(value, handler) value,
typedef enum V2MP_Supervisor_ActionType
{
	V2MP_SUPERVISOR_ACTION_LIST
} V2MP_Supervisor_ActionType;
#undef LIST_ITEM

typedef struct V2MP_Supervisor_Action
{
	V2MP_Supervisor_ActionType actionType;
	V2MP_Word args[4];
} V2MP_Supervisor_Action;

#define SVACTION_LOAD_WORD_ARG_ADDRESS(actionPtr) ((actionPtr)->args[0])
#define SVACTION_LOAD_WORD_ARG_DESTREG(actionPtr) ((actionPtr)->args[1])

#define SVACTION_STORE_WORD_ARG_ADDRESS(actionPtr) ((actionPtr)->args[0])
#define SVACTION_STORE_WORD_ARG_WORD(actionPtr) ((actionPtr)->args[1])

#define SVACTION_DPQ_ARG_PORT(actionPtr) ((actionPtr)->args[0])
#define SVACTION_DPQ_ARG_QUERY_TYPE(actionPtr) ((actionPtr)->args[1])

#define SVACTION_DEVDT_ARG_PORT(actionPtr) ((actionPtr)->args[0])
#define SVACTION_DEVDT_ARG_DS_ADDR(actionPtr) ((actionPtr)->args[1])
#define SVACTION_DEVDT_ARG_DS_SIZE(actionPtr) ((actionPtr)->args[2])
#define SVACTION_DEVDT_ARG_FLAGS(actionPtr) ((actionPtr)->args[3])
#define SVACTION_DEVDT_FLAG_IS_MB_WRITE (1 << 0)
#define SVACTION_DEVDT_FLAG_IS_IN_PROGRESS (1 << 1)
#define SVACTION_DEVDT_FLAG_IS_INDIRECT (1 << 2)

#define SVACTION_RELINQUISH_MAILBOX_ARG_PORT(actionPtr) ((actionPtr)->args[0])

#define SVACTION_USABLE_BYTE_COUNT_ARG_PORT(actionPtr) ((actionPtr)->args[0])

#define SVACTION_STACK_REG_FLAGS(actionPtr) ((actionPtr)->args[0])
#define SVACTION_STACK_IS_PUSH(actionPtr) ((actionPtr)->args[1])

bool V2MP_Supervisor_CreateActionLists(V2MP_Supervisor* supervisor);
void V2MP_Supervisor_DestroyActionLists(V2MP_Supervisor* supervisor);
V2MP_Supervisor_Action* V2MP_Supervisor_CreateNewAction(V2MP_Supervisor* supervisor);
V2MPSC_DoubleLL_Node* V2MP_Supervisor_CloneToOngoingAction(V2MP_Supervisor* supervisor, V2MPSC_DoubleLL_Node* createAfter, V2MP_Supervisor_Action* template);
bool V2MP_Supervisor_ResolveOutstandingActions(V2MP_Supervisor* supervisor);

#endif // V2MP_MODULES_SUPERVISOR_POSTINSTRUCTIONACTION_H
