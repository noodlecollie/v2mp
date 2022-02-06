#include "V2MPInternal/Modules/Device.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "V2MPInternal/Util/Util.h"
#include "Modules/Device_Internal.h"
#include "Modules/DevicePort_Internal.h"

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device)
{
	return device ? device->connectedPort != NULL : false;
}

bool V2MP_Device_ControlsConnectedMailbox(const V2MP_Device* device)
{
	return
		V2MP_Device_IsConnectedToPort(device) &&
		V2MP_DevicePort_GetMailboxController(device->connectedPort) == V2MP_DPMC_DEVICE;
}

V2MP_Word V2MP_Device_AddressOfConnectedPort(const V2MP_Device* device)
{
	return (device && device->connectedPort) ? V2MP_DevicePort_GetAddress(device->connectedPort) : 0;
}

void V2MP_Device_SetCallbacks(V2MP_Device* device, const V2MP_Device_Callbacks* callbacks)
{
	if ( !device )
	{
		return;
	}

	if ( callbacks )
	{
		device->callbacks = *callbacks;
	}
	else
	{
		V2MP_ZERO_STRUCT_PTR(&device->callbacks);
	}
}

void V2MP_Device_ClearCallbacks(V2MP_Device* device)
{
	V2MP_Device_SetCallbacks(device, NULL);
}

bool V2MP_Device_AllocateConnectedMailbox(V2MP_Device* device, size_t sizeInBytes)
{
	return V2MP_DevicePort_DeviceAllocateMailbox(device->connectedPort, sizeInBytes);
}

bool V2MP_Device_DeallocateConnectedMailbox(V2MP_Device* device)
{
	return V2MP_DevicePort_DeviceDeallocateMailbox(device->connectedPort);
}

size_t V2MP_Device_WriteToConnectedMailbox(V2MP_Device* device, const V2MP_Byte* data, size_t dataSize)
{
	if ( !V2MP_Device_ControlsConnectedMailbox(device) )
	{
		return 0;
	}

	return V2MP_CircularBuffer_WriteData(V2MP_DevicePort_GetMailbox(device->connectedPort), data, dataSize);
}

size_t V2MP_Device_ReadFromConnectedMailbox(V2MP_Device* device, V2MP_Byte* outBuffer, size_t outBufferSize)
{
	if ( !V2MP_Device_ControlsConnectedMailbox(device) )
	{
		return 0;
	}

	return V2MP_CircularBuffer_ReadData(V2MP_DevicePort_GetMailbox(device->connectedPort), outBuffer, outBufferSize);
}

size_t V2MP_Device_CopyFromConnectedMailbox(V2MP_Device* device, V2MP_Byte* outBuffer, size_t outBufferSize)
{
	if ( !V2MP_Device_ControlsConnectedMailbox(device) )
	{
		return 0;
	}

	return V2MP_CircularBuffer_CopyData(V2MP_DevicePort_GetMailbox(device->connectedPort), outBuffer, outBufferSize);
}

bool V2MP_Device_HasConnectedMailbox(const V2MP_Device* device)
{
	return (device && device->connectedPort) ? V2MP_DevicePort_HasMailbox(device->connectedPort) : false;
}

size_t V2MP_Device_FreeBytesInConnectedMailbox(const V2MP_Device* device)
{
	return (device && device->connectedPort) ? V2MP_DevicePort_MailboxBytesFree(device->connectedPort) : 0;
}

size_t V2MP_Device_UsedBytesInConnectedMailbox(const V2MP_Device* device)
{
	return (device && device->connectedPort) ? V2MP_DevicePort_MailboxBytesUsed(device->connectedPort) : 0;
}

bool V2MP_Device_IsConnectedMailboxEmpty(const V2MP_Device* device)
{
	return (device && device->connectedPort) ? V2MP_DevicePort_IsMailboxEmpty(device->connectedPort) : true;
}

bool V2MP_Device_IsConnectedMailboxFull(const V2MP_Device* device)
{
	return (device && device->connectedPort) ? V2MP_DevicePort_IsMailboxFull(device->connectedPort) : false;
}

bool V2MP_Device_RelinquishConnectedMailbox(V2MP_Device* device)
{
	if ( !V2MP_Device_ControlsConnectedMailbox(device) )
	{
		return false;
	}

	return V2MP_DevicePort_DeviceRelinquishMailbox(device->connectedPort);
}

size_t V2MP_Device_GetDataTransferSpeed(const V2MP_Device* device)
{
	return device ? device->dataTransferSpeed : 0;
}

void V2MP_Device_SetDataTransferSpeed(V2MP_Device* device, size_t bytesPerClockCycle)
{
	if ( !device )
	{
		return;
	}

	device->dataTransferSpeed = bytesPerClockCycle;
}
