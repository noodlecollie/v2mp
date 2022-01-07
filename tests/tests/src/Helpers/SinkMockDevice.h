#pragma once

#include <vector>
#include "Helpers/BaseMockDevice.h"
#include "V2MPInternal/Defs.h"

class SinkMockDevice : public BaseMockDevice
{
public:
	void ReadAllDataFromMailbox(std::vector<V2MP_Byte>& outBuffer);
};
