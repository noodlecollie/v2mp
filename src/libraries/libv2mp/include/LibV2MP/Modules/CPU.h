#ifndef V2MPINTERNAL_MODULES_CPU_H
#define V2MPINTERNAL_MODULES_CPU_H

#include <stdbool.h>
#include "LibV2MP/LibExport.gen.h"
#include "LibV2MP/Defs.h"

typedef struct V2MP_CPU V2MP_CPU;

typedef struct V2MP_CPU_SupervisorInterface
{
	void* supervisor;

	// Returns a fault word indicating the result of the operation.
	V2MP_Word (*fetchInstructionWord)(void* supervisor, V2MP_Word address, V2MP_Word* destReg);

	// LDST
	void (*requestLoadWordFromDS)(void* supervisor, V2MP_Word address, V2MP_RegisterIndex destReg);
	void (*requestStoreWordToDS)(void* supervisor, V2MP_Word address, V2MP_Word wordToStore);

	// STK
	void (*requestStackPush)(void* supervisor, V2MP_Word regFlags);
	void (*requestStackPop)(void* supervisor, V2MP_Word regFlags);

	// SIG
	void (*raiseSignal)(void* supervisor, V2MP_Word signal, V2MP_Word r1, V2MP_Word lr, V2MP_Word sp);

} V2MP_CPU_SupervisorInterface;

LIBV2MP_PUBLIC(V2MP_CPU*) V2MP_CPU_AllocateAndInit(void);
LIBV2MP_PUBLIC(void) V2MP_CPU_DeinitAndFree(V2MP_CPU* cpu);

LIBV2MP_PUBLIC(void) V2MP_CPU_SetSupervisorInterface(V2MP_CPU* cpu, const V2MP_CPU_SupervisorInterface* interface);
LIBV2MP_PUBLIC(void) V2MP_CPU_ResetSupervisorInterface(V2MP_CPU* cpu);

LIBV2MP_PUBLIC(void) V2MP_CPU_Reset(V2MP_CPU* cpu);
LIBV2MP_PUBLIC(bool) V2MP_CPU_ExecuteClockCycle(V2MP_CPU* cpu);

LIBV2MP_PUBLIC(void) V2MP_CPU_NotifyFault(V2MP_CPU* cpu, V2MP_Fault fault);
LIBV2MP_PUBLIC(bool) V2MP_CPU_HasFault(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetFaultWord(const V2MP_CPU* cpu);

LIBV2MP_PUBLIC(bool) V2MP_CPU_SetRegisterValue(V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value);
LIBV2MP_PUBLIC(bool) V2MP_CPU_SetRegisterValueAndUpdateSR(V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value);
LIBV2MP_PUBLIC(bool) V2MP_CPU_GetRegisterValue(const V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word* outValue);

LIBV2MP_PUBLIC(bool) V2MP_CPU_ExecuteSingleInstruction(V2MP_CPU* cpu, V2MP_Word instruction);

LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetProgramCounter(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(void) V2MP_CPU_SetProgramCounter(V2MP_CPU* cpu, V2MP_Word value);

LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetStatusRegister(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(void) V2MP_CPU_SetStatusRegister(V2MP_CPU* cpu, V2MP_Word value);

LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetLinkRegister(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(void) V2MP_CPU_SetLinkRegister(V2MP_CPU* cpu, V2MP_Word value);

LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetR0(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(void) V2MP_CPU_SetR0(V2MP_CPU* cpu, V2MP_Word value);

LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetR1(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(void) V2MP_CPU_SetR1(V2MP_CPU* cpu, V2MP_Word value);

LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetInstructionRegister(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(void) V2MP_CPU_SetInstructionRegister(V2MP_CPU* cpu, V2MP_Word value);

LIBV2MP_PUBLIC(V2MP_Word) V2MP_CPU_GetStackPointer(const V2MP_CPU* cpu);
LIBV2MP_PUBLIC(void) V2MP_CPU_SetStackPointer(V2MP_CPU* cpu, V2MP_Word value);

#endif // V2MPINTERNAL_MODULES_CPU_H
