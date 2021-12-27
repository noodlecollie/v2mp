#ifndef V2MP_MODULES_CPU_INTERNAL_H
#define V2MP_MODULES_CPU_INTERNAL_H

#include "V2MPInternal/Modules/CPU.h"

struct V2MP_CPURenameMe
{
	V2MP_Word pc;
	V2MP_Word sr;
	V2MP_Word lr;
	V2MP_Word r0;
	V2MP_Word r1;
	V2MP_Word ir;
	V2MP_Word fault;

	V2MP_CPURenameMe_SupervisorInterface supervisorInterface;
};

V2MP_Word* V2MP_CPURenameMe_GetRegisterPtr(V2MP_CPURenameMe* cpu, V2MP_Word regIndex);
const V2MP_Word* V2MP_CPURenameMe_GetRegisterConstPtr(const V2MP_CPURenameMe* cpu, V2MP_Word regIndex);

#endif // V2MP_MODULES_CPU_INTERNAL_H
