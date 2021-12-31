#pragma once

#include <string>
#include "Helpers/BaseMockDevice.h"

class EmitterMockDevice : public BaseMockDevice
{
public:
	size_t WriteToMailbox(const std::string& message);
};
