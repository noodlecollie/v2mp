#pragma once

#include <vector>
#include "Helpers/BaseMockDevice.h"
#include "V2MPInternal/Defs.h"

class SinkMockDevice : public BaseMockDevice
{
public:
	void CopyAllDataFromMailbox(std::vector<V2MP_Byte>& outBuffer);
	void ConsumeAllDataFromMailbox(std::vector<V2MP_Byte>& outBuffer);
};
