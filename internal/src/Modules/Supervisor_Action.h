#ifndef V2MP_MODULES_SUPERVISOR_ACTION_H
#define V2MP_MODULES_SUPERVISOR_ACTION_H

#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/Supervisor.h"

typedef enum V2MP_Supervisor_ActionType
{
	SVAT_LOAD_WORD = 0,
	SVAT_STORE_WORD
} V2MP_Supervisor_ActionType;

typedef struct V2MP_Supervisor_Action
{
	V2MP_Supervisor_ActionType actionType;
	V2MP_Word args[4];
} V2MP_Supervisor_Action;

#define SVACTION_LOAD_WORD_ARG_ADDRESS(actionPtr) ((actionPtr)->args[0])
#define SVACTION_LOAD_WORD_ARG_DESTREG(actionPtr) ((actionPtr)->args[1])

#define SVACTION_STORE_WORD_ARG_ADDRESS(actionPtr) ((actionPtr)->args[0])
#define SVACTION_STORE_WORD_ARG_WORD(actionPtr) ((actionPtr)->args[1])

bool V2MP_Supervisor_CreateActionList(V2MP_Supervisor* supervisor);
void V2MP_Supervisor_DestroyActionList(V2MP_Supervisor* supervisor);
V2MP_Supervisor_Action* V2MP_Supervisor_CreateNewAction(V2MP_Supervisor* supervisor);
bool V2MP_Supervisor_ResolveOutstandingActions(V2MP_Supervisor* supervisor);

#endif // V2MP_MODULES_SUPERVISOR_POSTINSTRUCTIONACTION_H
