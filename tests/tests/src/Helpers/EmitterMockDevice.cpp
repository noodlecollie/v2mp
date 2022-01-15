#include "Helpers/EmitterMockDevice.h"

size_t EmitterMockDevice::WriteToConnectedMailbox(const std::string& message)
{
	return V2MP_Device_WriteToConnectedMailbox(
		WrappedDevice(),
		reinterpret_cast<const V2MP_Byte*>(message.c_str()),
		message.length() + 1
	);
}

size_t EmitterMockDevice::WriteToConnectedMailbox(const void* data, size_t length)
{
	return V2MP_Device_WriteToConnectedMailbox(
		WrappedDevice(),
		static_cast<const V2MP_Byte*>(data),
		length
	);
}
