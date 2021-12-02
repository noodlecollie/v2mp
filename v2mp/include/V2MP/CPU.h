#ifndef V2MP_CPU_H
#define V2MP_CPU_H

#include <stdbool.h>
#include "V2MP/Defs.h"

typedef struct _V2MP_CPU
{
	V2MP_Word pc;
	V2MP_Word sr;
	V2MP_Word lr;
	V2MP_Word r0;
	V2MP_Word r1;
	V2MP_Word ir;

	struct _V2MP_MemoryStore* memory;
	V2MP_Word fault;
} V2MP_CPU;

void V2MP_CPU_Init(V2MP_CPU* cpu);
void V2MP_CPU_Deinit(V2MP_CPU* cpu);

// Not really representative of an actual CPU,
// which would do this over multiple clock cycles,
// but we simplify somewhat for ease of simulation.
bool V2MP_CPU_FetchDecodeAndExecute(V2MP_CPU* cpu);

bool V2MP_CPU_SetFault(V2MP_CPU* cpu, V2MP_Fault fault, V2MP_Word args);

#endif // V2MP_CPU_H
