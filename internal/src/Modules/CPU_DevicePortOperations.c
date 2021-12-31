#include "Modules/CPU_DevicePortOperations.h"

bool V2MP_CPU_DPO_UsableByteCount(V2MP_CPU* cpu)
{
	// TODO
	V2MP_CPU_NotifyFault(cpu, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_INI, V2MP_OP_DPO_OPERATION_TYPE(cpu->ir)));
	return true;
}

bool V2MP_CPU_DPO_RelinquishMailbox(V2MP_CPU* cpu)
{
	// TODO
	V2MP_CPU_NotifyFault(cpu, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_INI, V2MP_OP_DPO_OPERATION_TYPE(cpu->ir)));
	return true;
}

bool V2MP_CPU_DPO_Read(V2MP_CPU* cpu)
{
	if ( !cpu->supervisorInterface.requestDevicePortIndirectRead )
	{
		return false;
	}

	cpu->supervisorInterface.requestDevicePortIndirectRead(
		cpu->supervisorInterface.supervisor,
		cpu->r0,
		cpu->lr,
		cpu->r1
	);

	return true;
}

bool V2MP_CPU_DPO_Write(V2MP_CPU* cpu)
{
	if ( !cpu->supervisorInterface.requestDevicePortIndirectWrite )
	{
		return false;
	}

	// TODO
	V2MP_CPU_NotifyFault(cpu, V2MP_CPU_MAKE_FAULT_WORD(V2MP_FAULT_INI, V2MP_OP_DPO_OPERATION_TYPE(cpu->ir)));
	return true;
}
