#include "Helpers/EmitterMockDevice.h"

size_t EmitterMockDevice::WriteToMailbox(const std::string& message)
{
	return V2MP_Device_WriteToConnectedMailbox(
		WrappedDevice(),
		reinterpret_cast<const uint8_t*>(message.c_str()),
		message.length() + 1
	);
}
