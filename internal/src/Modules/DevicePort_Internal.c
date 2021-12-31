#include "Modules/DevicePort_Internal.h"

void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address)
{
	if ( !port )
	{
		return;
	}

	port->address = address;
}
