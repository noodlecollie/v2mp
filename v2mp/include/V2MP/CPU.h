#ifndef V2MP_CPU_H
#define V2MP_CPU_H

#include <stdbool.h>
#include <stddef.h>
#include "V2MP/LibExport.h"
#include "V2MP/Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct V2MP_CPU V2MP_CPU;
struct V2MP_MemoryStore;
struct V2MP_DevicePortStore;

API_V2MP size_t V2MP_CPU_Footprint(void);
API_V2MP V2MP_CPU* V2MP_CPU_AllocateAndInit(void);
API_V2MP void V2MP_CPU_DeinitAndFree(V2MP_CPU* cpu);

API_V2MP void V2MP_CPU_Reset(V2MP_CPU* cpu);

// Not really representative of an actual CPU,
// which would do this over multiple clock cycles,
// but we simplify somewhat for ease of simulation.
API_V2MP bool V2MP_CPU_FetchDecodeAndExecuteInstruction(V2MP_CPU* cpu);

// The memory store is not owned by the CPU.
API_V2MP struct V2MP_MemoryStore* V2MP_CPU_GetMemoryStore(V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetMemoryStore(V2MP_CPU* cpu, struct V2MP_MemoryStore* memory);

// The device port store is not owned by the CPU.
API_V2MP struct V2MP_DevicePortStore* V2MP_CPU_GetDevicePortStore(V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetDevicePortStore(V2MP_CPU* cpu, struct V2MP_DevicePortStore* devicePorts);

// The following functions are primarily for debugging:

// Does not increment the program counter.
API_V2MP bool V2MP_CPU_ExecuteInstruction(V2MP_CPU* cpu, V2MP_Word instruction);

API_V2MP V2MP_Word V2MP_CPU_GetProgramCounter(const V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetProgramCounter(V2MP_CPU* cpu, V2MP_Word value);

API_V2MP V2MP_Word V2MP_CPU_GetStatusRegister(const V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetStatusRegister(V2MP_CPU* cpu, V2MP_Word value);

API_V2MP V2MP_Word V2MP_CPU_GetLinkRegister(const V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetLinkRegister(V2MP_CPU* cpu, V2MP_Word value);

API_V2MP V2MP_Word V2MP_CPU_GetR0(const V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetR0(V2MP_CPU* cpu, V2MP_Word value);

API_V2MP V2MP_Word V2MP_CPU_GetR1(const V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetR1(V2MP_CPU* cpu, V2MP_Word value);

API_V2MP V2MP_Word V2MP_CPU_GetInstructionRegister(const V2MP_CPU* cpu);
API_V2MP void V2MP_CPU_SetInstructionRegister(V2MP_CPU* cpu, V2MP_Word value);

API_V2MP V2MP_Word V2MP_CPU_GetFault(const V2MP_CPU* cpu);
API_V2MP bool V2MP_CPU_SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_CPU_H
