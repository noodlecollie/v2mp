#ifndef V2MP_MODULES_CPU_H
#define V2MP_MODULES_CPU_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_CPU V2MP_CPU;

typedef struct V2MP_CPU_SupervisorInterface
{
	void* supervisor;

	// Returns a fault word indicating the result of the operation.
	V2MP_Word (*fetchInstructionWord)(void* supervisor, V2MP_Word address, V2MP_Word* destReg);

	// LDST
	void (*requestLoadWordFromDS)(void* supervisor, V2MP_Word address, V2MP_RegisterIndex destReg);
	void (*requestStoreWordToDS)(void* supervisor, V2MP_Word address, V2MP_Word wordToStore);

	// DPQ
	void (*performDevicePortQuery)(void* supervisor, V2MP_Word port, V2MP_Word queryType);

	// DPO
	void (*requestDevicePortDirectRead)(
		void* supervisor,
		V2MP_Word port
	);

	void (*requestDevicePortIndirectRead)(
		void* supervisor,
		V2MP_Word port,
		V2MP_Word dsDestAddress,
		V2MP_Word dsMaxBytes
	);

	void (*requestDevicePortDirectWrite)(
		void* supervisor,
		V2MP_Word port
	);

	void (*requestDevicePortIndirectWrite)(
		void* supervisor,
		V2MP_Word port,
		V2MP_Word dsSrcAddress,
		V2MP_Word dsMaxBytes
	);

	void (*requestRelinquishMailbox)(void* supervisor, V2MP_Word port);
	void (*requestUsableByteCount)(void* supervisor, V2MP_Word port);

	// STK
	void (*requestStackPush)(void* supervisor, V2MP_Word regFlags);
	void (*requestStackPop)(void* supervisor, V2MP_Word regFlags);

} V2MP_CPU_SupervisorInterface;

V2MP_CPU* V2MP_CPU_AllocateAndInit(void);
void V2MP_CPU_DeinitAndFree(V2MP_CPU* cpu);

void V2MP_CPU_SetSupervisorInterface(V2MP_CPU* cpu, const V2MP_CPU_SupervisorInterface* interface);
void V2MP_CPU_ResetSupervisorInterface(V2MP_CPU* cpu);

void V2MP_CPU_Reset(V2MP_CPU* cpu);
bool V2MP_CPU_ExecuteClockCycle(V2MP_CPU* cpu);

void V2MP_CPU_NotifyFault(V2MP_CPU* cpu, V2MP_Fault fault);
bool V2MP_CPU_HasFault(const V2MP_CPU* cpu);
V2MP_Word V2MP_CPU_GetFaultWord(const V2MP_CPU* cpu);

bool V2MP_CPU_SetRegisterValue(V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value);
bool V2MP_CPU_SetRegisterValueAndUpdateSR(V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word value);
bool V2MP_CPU_GetRegisterValue(const V2MP_CPU* cpu, V2MP_RegisterIndex regIndex, V2MP_Word* outValue);

bool V2MP_CPU_ExecuteSingleInstruction(V2MP_CPU* cpu, V2MP_Word instruction);

V2MP_Word V2MP_CPU_GetProgramCounter(const V2MP_CPU* cpu);
void V2MP_CPU_SetProgramCounter(V2MP_CPU* cpu, V2MP_Word value);

V2MP_Word V2MP_CPU_GetStatusRegister(const V2MP_CPU* cpu);
void V2MP_CPU_SetStatusRegister(V2MP_CPU* cpu, V2MP_Word value);

V2MP_Word V2MP_CPU_GetLinkRegister(const V2MP_CPU* cpu);
void V2MP_CPU_SetLinkRegister(V2MP_CPU* cpu, V2MP_Word value);

V2MP_Word V2MP_CPU_GetR0(const V2MP_CPU* cpu);
void V2MP_CPU_SetR0(V2MP_CPU* cpu, V2MP_Word value);

V2MP_Word V2MP_CPU_GetR1(const V2MP_CPU* cpu);
void V2MP_CPU_SetR1(V2MP_CPU* cpu, V2MP_Word value);

V2MP_Word V2MP_CPU_GetInstructionRegister(const V2MP_CPU* cpu);
void V2MP_CPU_SetInstructionRegister(V2MP_CPU* cpu, V2MP_Word value);

V2MP_Word V2MP_CPU_GetStackPointer(const V2MP_CPU* cpu);
void V2MP_CPU_SetStackPointer(V2MP_CPU* cpu, V2MP_Word value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_MODULES_CPU_H
