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
	void* supervisor;

	// Returns a fault word indicating the result of the operation.
	V2MP_Word (*fetchInstructionWord)(void* supervisor, V2MP_Word address, V2MP_Word* destReg);

	void (*requestLoadWordFromDS)(void* supervisor, V2MP_Word address, V2MP_RegisterIndex destReg);
	void (*requestStoreWordToDS)(void* supervisor, V2MP_Word address, V2MP_Word wordToStore);
} V2MP_CPURenameMe_SupervisorInterface;

V2MP_CPURenameMe* V2MP_CPURenameMe_AllocateAndInit(void);
void V2MP_CPURenameMe_DeinitAndFree(V2MP_CPURenameMe* cpu);

void V2MP_CPURenameMe_SetSupervisorInterface(V2MP_CPURenameMe* cpu, const V2MP_CPURenameMe_SupervisorInterface* interface);
void V2MP_CPURenameMe_ResetSupervisorInterface(V2MP_CPURenameMe* cpu);

void V2MP_CPURenameMe_Reset(V2MP_CPURenameMe* cpu);
bool V2MP_CPURenameMe_ExecuteClockCycle(V2MP_CPURenameMe* cpu);

void V2MP_CPURenameMe_NotifyFault(V2MP_CPURenameMe* cpu, V2MP_Fault fault);
bool V2MP_CPURenameMe_HasFault(const V2MP_CPURenameMe* cpu);
V2MP_Word V2MP_CPURenameMe_GetFaultWord(const V2MP_CPURenameMe* cpu);

bool V2MP_CPURenameMe_SetRegisterValue(V2MP_CPURenameMe* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value);
bool V2MP_CPURenameMe_GetRegisterValue(const V2MP_CPURenameMe* cpu, V2MP_RegisterIndex regIndex, V2MP_Word* outValue);

// The functions below are for debugging/introspection. They should not be used in normal operation.

bool V2MP_CPURenameMe_ExecuteSingleInstruction(V2MP_CPURenameMe* cpu, V2MP_Word instruction);

V2MP_Word V2MP_CPURenameMe_GetProgramCounter(const V2MP_CPURenameMe* cpu);
void V2MP_CPURenameMe_SetProgramCounter(V2MP_CPURenameMe* cpu, V2MP_Word value);

V2MP_Word V2MP_CPURenameMe_GetStatusRegister(const V2MP_CPURenameMe* cpu);
void V2MP_CPURenameMe_SetStatusRegister(V2MP_CPURenameMe* cpu, V2MP_Word value);

V2MP_Word V2MP_CPURenameMe_GetLinkRegister(const V2MP_CPURenameMe* cpu);
void V2MP_CPURenameMe_SetLinkRegister(V2MP_CPURenameMe* cpu, V2MP_Word value);

V2MP_Word V2MP_CPURenameMe_GetR0(const V2MP_CPURenameMe* cpu);
void V2MP_CPURenameMe_SetR0(V2MP_CPURenameMe* cpu, V2MP_Word value);

V2MP_Word V2MP_CPURenameMe_GetR1(const V2MP_CPURenameMe* cpu);
void V2MP_CPURenameMe_SetR1(V2MP_CPURenameMe* cpu, V2MP_Word value);

V2MP_Word V2MP_CPURenameMe_GetInstructionRegister(const V2MP_CPURenameMe* cpu);
void V2MP_CPURenameMe_SetInstructionRegister(V2MP_CPURenameMe* cpu, V2MP_Word value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_CPU_H
