#include "V2MPInternal/Modules/Device.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Components/CircularBuffer.h"
#include "V2MPInternal/Util/Util.h"
#include "Modules/Device_Internal.h"

bool V2MP_Device_IsConnectedToPort(const V2MP_Device* device)
{
	return device ? device->connectedPort != NULL : false;
}

bool V2MP_Device_ControlsConnectedMailbox(const V2MP_Device* device)
{
	return
		V2MP_Device_IsConnectedToPort(device) &&
		V2MP_DevicePort_GetMailboxController(device->connectedPort) == V2MP_MBC_DEVICE;
}

V2MP_Word V2MP_Device_AddressOfConnectedPort(const V2MP_Device* device)
{
	return (device && device->connectedPort) ? V2MP_DevicePort_GetAddress(device->connectedPort) : 0;
}

void V2MP_Device_SetExtInterface(V2MP_Device* device, const V2MP_Device_ExtInterface* interface)
{
	if ( !device )
	{
		return;
	}

	if ( interface )
	{
		device->extInterface = *interface;
	}
	else
	{
		V2MP_ZERO_STRUCT_PTR(&device->extInterface);
	}
}

void V2MP_Device_ClearExtInterface(V2MP_Device* device)
{
	V2MP_Device_SetExtInterface(device, NULL);
}

bool V2MP_Device_Poll(V2MP_Device* device)
{
	if ( !device || !device->extInterface.onPoll )
	{
		return false;
	}

	device->extInterface.onPoll(device->extInterface.userData, device);
	return true;
}

bool V2MP_Device_AllocateConnectedMailbox(V2MP_Device* device, size_t sizeInBytes)
{
	if ( !V2MP_Device_ControlsConnectedMailbox(device) )
	{
		return false;
	}

	return V2MP_DevicePort_DeviceAllocateMailbox(device->connectedPort, sizeInBytes);
}

bool V2MP_Device_DeallocateConnectedMailbox(V2MP_Device* device)
{
	if ( !V2MP_Device_ControlsConnectedMailbox(device) )
	{
		return false;
	}

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

void V2MP_Device_RelinquishConnectedMailbox(V2MP_Device* device)
{
	if ( !V2MP_Device_ControlsConnectedMailbox(device) )
	{
		return;
	}

	V2MP_DevicePort_SetMailboxController(device->connectedPort, V2MP_MBC_PROGRAM);
}
