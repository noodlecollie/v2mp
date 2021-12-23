#include <stdlib.h>
#include "V2MP/DevicePort.h"

struct V2MP_DevicePort
{
	int dummy;
};

size_t V2MP_DevicePort_Footprint(void)
{
	return sizeof(V2MP_DevicePort);
}

V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void)
{
	return (V2MP_DevicePort*)calloc(1, sizeof(V2MP_DevicePort));
}

void V2MP_DevicePort_DeinitAndFree(V2MP_DevicePort* port)
{
	if ( !port )
	{
		return;
	}

	free(port);
}
