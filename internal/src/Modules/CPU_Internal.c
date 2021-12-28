#include <stddef.h>
#include "Modules/CPU_Internal.h"

V2MP_Word* V2MP_CPU_GetRegisterPtr(V2MP_CPU* cpu, V2MP_Word regIndex)
{
	return (V2MP_Word*)V2MP_CPU_GetRegisterConstPtr(cpu, regIndex);
}

const V2MP_Word* V2MP_CPU_GetRegisterConstPtr(const V2MP_CPU* cpu, V2MP_Word regIndex)
{
	if ( !cpu )
	{
		return NULL;
	}

	switch ( V2MP_REGID_MASK(regIndex) )
	{
		case V2MP_REGID_R0:
		{
			return &cpu->r0;
		}

		case V2MP_REGID_R1:
		{
			return &cpu->r1;
		}

		case V2MP_REGID_LR:
		{
			return &cpu->lr;
		}

		case V2MP_REGID_PC:
		{
			return &cpu->pc;
		}

		default:
		{
			return NULL;
		}
	}
}
