#include "Helpers/SinkMockDevice.h"

void SinkMockDevice::CopyAllDataFromMailbox(std::vector<V2MP_Byte>& outBuffer)
{
	outBuffer.clear();
	outBuffer.resize(V2MP_Device_UsedBytesInConnectedMailbox(WrappedDevice()));
	V2MP_Device_CopyFromConnectedMailbox(WrappedDevice(), outBuffer.data(), outBuffer.size());
}

void SinkMockDevice::ConsumeAllDataFromMailbox(std::vector<V2MP_Byte>& outBuffer)
{
	outBuffer.clear();
	outBuffer.resize(V2MP_Device_UsedBytesInConnectedMailbox(WrappedDevice()));
	V2MP_Device_ReadFromConnectedMailbox(WrappedDevice(), outBuffer.data(), outBuffer.size());
}
