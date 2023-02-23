#ifndef V2MPINTERNAL_MODULES_CPU_INTERNAL_H
#define V2MPINTERNAL_MODULES_CPU_INTERNAL_H

#include "LibV2MPInternal/Modules/CPU.h"

struct V2MP_CPU
{
	V2MP_Word pc;
	V2MP_Word sr;
	V2MP_Word lr;
	V2MP_Word r0;
	V2MP_Word r1;
	V2MP_Word ir;
	V2MP_Word sp;
	V2MP_Word fault;

	V2MP_CPU_SupervisorInterface supervisorInterface;
};

V2MP_Word* V2MP_CPU_GetRegisterPtr(V2MP_CPU* cpu, V2MP_Word regIndex);
const V2MP_Word* V2MP_CPU_GetRegisterConstPtr(const V2MP_CPU* cpu, V2MP_Word regIndex);

#endif // V2MPINTERNAL_MODULES_CPU_INTERNAL_H
