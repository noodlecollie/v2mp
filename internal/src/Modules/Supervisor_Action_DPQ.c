#include "Modules/Supervisor_Action_DPQ.h"
#include "Modules/Supervisor_Internal.h"
#include "V2MPInternal/Modules/Mainboard.h"
#include "V2MPInternal/Modules/DevicePortCollection.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Modules/CPU.h"

static inline V2MP_Word Query_IsConnected(V2MP_DevicePort* port)
{
	return (port && V2MP_DevicePort_HasConnectedDevice(port))
		? 0
		: V2MP_CPU_SR_Z;
}

static inline V2MP_Word Query_IsReadableAndNotBusy(V2MP_DevicePort* port)
{
	return (port && V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_READABLE && !V2MP_DevicePort_IsMailboxBusy(port))
		? 0
		: V2MP_CPU_SR_Z;
}

static inline V2MP_Word Query_IsWriteableAndNotBusy(V2MP_DevicePort* port)
{
	return (port && V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_WRITEABLE && !V2MP_DevicePort_IsMailboxBusy(port))
		? 0
		: V2MP_CPU_SR_Z;
}

static inline V2MP_Word Query_IsExhausted(V2MP_DevicePort* port)
{
	return (port && V2MP_DevicePort_GetMailboxState(port) == V2MP_DPMS_EXHAUSTED)
		? 0
		: V2MP_CPU_SR_Z;
}

static inline V2MP_Word Query_IsBusy(V2MP_DevicePort* port)
{
	return (port && V2MP_DevicePort_IsMailboxBusy(port))
		? 0
		: V2MP_CPU_SR_Z;
}

static inline V2MP_Word Query_IsControlledByProgram(V2MP_DevicePort* port)
{
	return (port && V2MP_DevicePort_GetMailboxController(port) == V2MP_DMBC_PROGRAM)
		? 0
		: V2MP_CPU_SR_Z;
}

static bool SwitchQuery(V2MP_CPU* cpu, V2MP_DevicePort* port, V2MP_DevicePortQueryType queryType)
{
	V2MP_Word sr = 0;

	switch ( queryType )
	{
		case V2MP_DPQT_CONNECTED:
		{
			sr = Query_IsConnected(port);
			break;
		}

		case V2MP_DPQT_READABLE_NOT_BUSY:
		{
			sr = Query_IsReadableAndNotBusy(port);
			break;
		}

		case V2MP_DPQT_WRITEABLE_NOT_BUSY:
		{
			sr = Query_IsWriteableAndNotBusy(port);
			break;
		}

		case V2MP_DPQT_EXHAUSTED:
		{
			sr = Query_IsExhausted(port);
			break;
		}

		case V2MP_DPQT_BUSY:
		{
			sr = Query_IsBusy(port);
			break;
		}

		case V2MP_DPQT_CONTROLLED_BY_PROGRAM:
		{
			sr = Query_IsControlledByProgram(port);
			break;
		}

		default:
		{
			return false;
		}
	}

	V2MP_CPU_SetStatusRegister(cpu, sr);
	return true;
}

bool V2MP_Supervisor_HandlePerformDevicePortQueryImpl(V2MP_Supervisor* supervisor, V2MP_Word port, V2MP_DevicePortQueryType queryType)
{
	bool success = false;

	do
	{
		V2MP_Mainboard* mainboard;
		V2MP_DevicePortCollection* dpc;
		V2MP_CPU* cpu;

		mainboard = V2MP_Supervisor_GetMainboard(supervisor);

		if ( !mainboard )
		{
			break;
		}

		dpc = V2MP_Mainboard_GetDevicePortCollection(mainboard);

		if ( !dpc )
		{
			break;
		}

		cpu = V2MP_Mainboard_GetCPU(mainboard);

		if ( !cpu )
		{
			break;
		}

		success = SwitchQuery(cpu, V2MP_DevicePortCollection_GetPort(dpc, port), queryType);
	}
	while ( false );

	return success;
}
