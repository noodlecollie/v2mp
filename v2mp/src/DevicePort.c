#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "V2MP/DevicePort.h"

struct V2MP_DevicePort
{
	int dummy;
};

static inline void FreeMailbox(V2MP_DevicePort* port)
{
	// TODO
	(void)port;
}

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

	FreeMailbox(port);
	free(port);
}

size_t V2MP_DevicePort_GetMailboxSize(V2MP_DevicePort* port)
{
	// TODO
	return 0;
	(void)port;
}

size_t V2MP_DevicePort_GetNumberOfBytesInMailbox(V2MP_DevicePort* port)
{
	// TODO
	return 0;
	(void)port;
}

bool V2MP_DevicePort_AllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes)
{
	// TODO
	(void)port;
	(void)sizeInBytes;
	return false;
}

void V2MP_DevicePort_DeallocateMailbox(V2MP_DevicePort* port)
{
	V2MP_DevicePort_AllocateMailbox(port, 0);
}

size_t V2MP_DevicePort_WriteToMailbox(V2MP_DevicePort* port, const V2MP_Byte* data, size_t dataSize)
{
	// TODO
	(void)port;
	(void)data;
	(void)dataSize;
	return 0;
}

size_t V2MP_DevicePort_ReadFromMailbox(V2MP_DevicePort* port, V2MP_Byte* buffer, size_t bufferSize)
{
	// TODO
	(void)port;
	(void)buffer;
	(void)bufferSize;
	return 0;
}
