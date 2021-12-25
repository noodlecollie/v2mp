#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "V2MP/DevicePort.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "V2MPInternal/Util/Util.h"

struct V2MP_DevicePort
{
	V2MPI_CircularBuffer* mailbox;
	V2MP_Word address;
};

static inline void FreeMailbox(V2MP_DevicePort* port)
{
	if ( port->mailbox )
	{
		V2MPI_CircularBuffer_DeinitAndFree(port->mailbox);
		port->mailbox = NULL;
	}
}

size_t V2MP_DevicePort_Footprint(void)
{
	return sizeof(V2MP_DevicePort);
}

V2MP_DevicePort* V2MP_DevicePort_AllocateAndInit(void)
{
	return V2MP_CALLOC_STRUCT(V2MP_DevicePort);
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

V2MP_Word V2MP_DevicePort_GetAddress(V2MP_DevicePort* port)
{
	return port ? port->address : 0;
}

void V2MP_DevicePort_SetAddress(V2MP_DevicePort* port, V2MP_Word address)
{
	if ( !port )
	{
		return;
	}

	port->address = address;
}

size_t V2MP_DevicePort_GetMailboxSize(V2MP_DevicePort* port)
{
	return port ? V2MPI_CircularBuffer_Capacity(port->mailbox) : 0;
}

size_t V2MP_DevicePort_GetMailboxMessageLength(V2MP_DevicePort* port)
{
	return port ? V2MPI_CircularBuffer_BytesUsed(port->mailbox) : 0;
}

size_t V2MP_DevicePort_GetMailboxFreeSpace(V2MP_DevicePort* port)
{
	return port ? V2MPI_CircularBuffer_BytesFree(port->mailbox) : 0;
}

bool V2MP_DevicePort_AllocateMailbox(V2MP_DevicePort* port, size_t sizeInBytes)
{
	if ( !port )
	{
		return false;
	}

	FreeMailbox(port);

	if ( sizeInBytes < 1 )
	{
		return true;
	}

	port->mailbox = V2MPI_CircularBuffer_AllocateAndInit(sizeInBytes);

	return port->mailbox != NULL;
}

void V2MP_DevicePort_DeallocateMailbox(V2MP_DevicePort* port)
{
	V2MP_DevicePort_AllocateMailbox(port, 0);
}

size_t V2MP_DevicePort_WriteToMailbox(V2MP_DevicePort* port, const V2MP_Byte* data, size_t dataSize)
{
	return port ? V2MPI_CircularBuffer_WriteData(port->mailbox, data, dataSize) : 0;
}

size_t V2MP_DevicePort_ReadFromMailbox(V2MP_DevicePort* port, V2MP_Byte* buffer, size_t bufferSize)
{
	return port ? V2MPI_CircularBuffer_ReadData(port->mailbox, buffer, bufferSize) : 0;
}
