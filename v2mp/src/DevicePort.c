#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "V2MP/DevicePort.h"
#include "V2MP/MemoryStore.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "V2MPInternal/Util/Util.h"
#include "V2MPInternal/Util/Heap.h"

typedef struct DataTransferInfo
{
	bool transferOngoing;
	V2MP_MemoryStore* memoryStore;
	size_t dsAddress;
	size_t dsBufferSize;
	bool writingToDS;
} DataTransferInfo;

struct V2MP_DevicePort
{
	V2MP_Word address;
	V2MP_CircularBuffer* mailbox;
	V2MP_DevicePort_MailboxState mailboxState;
	DataTransferInfo currentDataTransfer;
	size_t dataTransferSpeed;
};

static inline void FreeMailbox(V2MP_DevicePort* port)
{
	if ( port->mailbox )
	{
		V2MP_CircularBuffer_DeinitAndFree(port->mailbox);
		port->mailbox = NULL;
	}
}

static inline bool HasActiveDataTransfer(const V2MP_DevicePort* port)
{
	return port && port->currentDataTransfer.transferOngoing;
}

static inline void ClearActiveDataTransfer(V2MP_DevicePort* port)
{
	if ( port )
	{
		V2MP_ZERO_STRUCT_PTR(&port->currentDataTransfer);
	}
}

static size_t PerformDataTransferToDS(V2MP_DevicePort* port, size_t bytesToTransfer, V2MP_Fault* outFault)
{
	size_t totalBytesTransferred = 0;

	while ( bytesToTransfer > 0 )
	{
		size_t numBytesToTransferThisIteration;
		size_t numBytesTransferredByCall = 0;
		V2MP_Fault localFault = V2MP_FAULT_NONE;

		numBytesToTransferThisIteration = V2MP_CircularBuffer_NumSequentialBytesReadableFromTail(port->mailbox);

		if ( numBytesToTransferThisIteration > bytesToTransfer )
		{
			numBytesToTransferThisIteration = bytesToTransfer;
		}

		if ( !V2MP_MemoryStore_WriteBytesToDS(
				port->currentDataTransfer.memoryStore,
				(V2MP_Word)port->currentDataTransfer.dsAddress,
				V2MP_CircularBuffer_Tail(port->mailbox),
				numBytesToTransferThisIteration,
				&numBytesTransferredByCall,
				&localFault) )
		{
			// Should never happen, but just cancel.
			break;
		}

		V2MP_CircularBuffer_DiscardBytes(port->mailbox, numBytesTransferredByCall);
		totalBytesTransferred += numBytesTransferredByCall;
		bytesToTransfer -= V2MP_MIN(numBytesTransferredByCall, bytesToTransfer);

		if ( localFault != V2MP_FAULT_NONE )
		{
			if ( outFault )
			{
				*outFault = localFault;
			}

			break;
		}
	}

	return totalBytesTransferred;
}

static void HandleDataTransferToDS(V2MP_DevicePort* port, V2MP_Fault* outFault)
{
	DataTransferInfo* dt = &port->currentDataTransfer;
	size_t bytesToTransfer;
	size_t bytesTransferred;

	bytesToTransfer = V2MP_CircularBuffer_BytesUsed(port->mailbox);

	if ( port->dataTransferSpeed > 0 && bytesToTransfer > port->dataTransferSpeed )
	{
		bytesToTransfer = port->dataTransferSpeed;
	}

	bytesTransferred = PerformDataTransferToDS(port, bytesToTransfer, outFault);

	dt->dsAddress += bytesTransferred;
	dt->dsBufferSize -= V2MP_MIN(bytesTransferred, dt->dsBufferSize);

	if ( dt->dsBufferSize < 1 )
	{
		dt->transferOngoing = false;
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

V2MP_Word V2MP_DevicePort_GetAddress(const V2MP_DevicePort* port)
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

size_t V2MP_DevicePort_GetMailboxSize(const V2MP_DevicePort* port)
{
	return port ? V2MP_CircularBuffer_Capacity(port->mailbox) : 0;
}

size_t V2MP_DevicePort_GetMailboxMessageLength(const V2MP_DevicePort* port)
{
	return port ? V2MP_CircularBuffer_BytesUsed(port->mailbox) : 0;
}

size_t V2MP_DevicePort_GetMailboxFreeSpace(const V2MP_DevicePort* port)
{
	return port ? V2MP_CircularBuffer_BytesFree(port->mailbox) : 0;
}

V2MP_DevicePort_MailboxState V2MP_DevicePort_GetMailboxState(const V2MP_DevicePort* port)
{
	return port ? port->mailboxState : V2MP_DPMS_UNAVAILABLE;
}

V2MP_DevicePort_MailboxController V2MP_DevicePort_GetMailboxController(const V2MP_DevicePort* port)
{
	if ( !port )
	{
		// Make sure the program never owns invalid ports.
		return V2MP_DPMC_DEVICE;
	}

	return port->mailboxState == V2MP_DPMS_UNAVAILABLE ? V2MP_DPMC_DEVICE : V2MP_DPMC_PROGRAM;
}

size_t V2MP_DevicePort_GetMailboxDataTransferSpeed(const V2MP_DevicePort* port)
{
	return port ? port->dataTransferSpeed : 0;
}

void V2MP_DevicePort_SetMailboxDataTransferSpeed(V2MP_DevicePort* port, size_t speed)
{
	if ( !port )
	{
		return;
	}

	port->dataTransferSpeed = speed;
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

	port->mailbox = V2MP_CircularBuffer_AllocateAndInit(sizeInBytes);

	return port->mailbox != NULL;
}

void V2MP_DevicePort_DeallocateMailbox(V2MP_DevicePort* port)
{
	V2MP_DevicePort_AllocateMailbox(port, 0);
}

bool V2MP_DevicePort_BeginReadFromMailbox(
	V2MP_DevicePort* port,
	struct V2MP_MemoryStore* memoryStore,
	V2MP_Word destAddress,
	V2MP_Word maxBytesToRead
)
{
	DataTransferInfo* dt;

	if ( !port || !memoryStore || HasActiveDataTransfer(port) )
	{
		return false;
	}

	ClearActiveDataTransfer(port);

	dt = &port->currentDataTransfer;

	dt->memoryStore = memoryStore;
	dt->dsAddress = destAddress;
	dt->dsBufferSize = maxBytesToRead;
	dt->writingToDS = true;

	return true;
}

bool V2MP_DevicePort_BeginWriteToMailbox(
	V2MP_DevicePort* port,
	struct V2MP_MemoryStore* memoryStore,
	V2MP_Word srcAddress,
	V2MP_Word maxBytesToWrite
)
{
	DataTransferInfo* dt;

	if ( !port || !memoryStore || HasActiveDataTransfer(port) )
	{
		return false;
	}

	ClearActiveDataTransfer(port);

	dt = &port->currentDataTransfer;

	dt->memoryStore = memoryStore;
	dt->dsAddress = srcAddress;
	dt->dsBufferSize = maxBytesToWrite;
	dt->writingToDS = false;

	return true;
}

bool V2MP_DevicePort_ExecuteCycle(V2MP_DevicePort* port, V2MP_Fault* outFault)
{
	if ( outFault )
	{
		*outFault = V2MP_FAULT_NONE;
	}

	if ( !port )
	{
		return false;
	}

	if ( HasActiveDataTransfer(port) )
	{
		if ( port->currentDataTransfer.writingToDS )
		{
			HandleDataTransferToDS(port, outFault);
		}
		else
		{
			// TODO
			return false;
		}
	}

	return true;
}
