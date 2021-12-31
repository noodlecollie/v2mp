#include "V2MPInternal/Modules/Device.h"
#include "V2MPInternal/Util/Heap.h"
#include "Modules/Device_Internal.h"

V2MP_Device* V2MP_Device_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_Device);
}

void V2MP_Device_DeinitAndFree(V2MP_Device* device)
{
	if ( !device )
	{
		return;
	}

	V2MP_FREE(device);
}

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device)
{
	return device ? device->connectedToPort : false;
}

V2MP_Word V2MP_Device_ConnectedPortAddress(const V2MP_Device* device)
{
	return (device && device->connectedToPort) ? device->portAddress : 0;
}
