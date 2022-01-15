#pragma once

#include <string>
#include "Helpers/BaseMockDevice.h"

class EmitterMockDevice : public BaseMockDevice
{
public:
	size_t WriteToConnectedMailbox(const std::string& message);
	size_t WriteToConnectedMailbox(const void* data, size_t length);

	template<std::size_t N>
	inline size_t WriteToConnectedMailbox(const V2MP_Byte (&data)[N])
	{
		return WriteToConnectedMailbox(static_cast<const void*>(data), N);
	}
};
