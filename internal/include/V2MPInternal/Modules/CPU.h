#ifndef V2MP_MODULES_CPU_H
#define V2MP_MODULES_CPU_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_CPURenameMe V2MP_CPURenameMe;

typedef struct V2MP_CPURenameMe_SupervisorInterface
{
	V2MP_Word (*fetchInstructionWord)(V2MP_Word address);
	void (*requestLoadWordFromDS)(V2MP_Word address, V2MP_Word* destReg);
	void (*requestStoreWordToDS)(V2MP_Word address, V2MP_Word wordToStore);
} V2MP_CPURenameMe_SupervisorInterface;

V2MP_CPURenameMe* V2MP_CPURenameMe_AllocateAndInit(void);
void V2MP_CPURenameMe_DeinitAndFree(V2MP_CPURenameMe* cpu);

void V2MP_CPURenameMe_SetSupervisorInterface(V2MP_CPURenameMe* cpu, const V2MP_CPURenameMe_SupervisorInterface* interface);

void V2MP_CPURenameMe_Reset(V2MP_CPURenameMe* cpu);

void V2MP_CPURenameMe_NotifyFault(V2MP_CPURenameMe* cpu, V2MP_Fault fault, V2MP_Word args);
bool V2MP_CPURenameMe_HasFault(V2MP_CPURenameMe* cpu);
V2MP_Word V2MP_CPURenameMe_GetFaultWord(V2MP_CPURenameMe* cpu);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_CPU_H
