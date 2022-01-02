#pragma once

#include "V2MPInternal/Modules/Device.h"
#include "V2MPInternal/Modules/DevicePort.h"
#include "V2MPInternal/Components/CircularBuffer.h"

class BaseMockDevice
{
public:
	BaseMockDevice();
	~BaseMockDevice();

	BaseMockDevice(const BaseMockDevice& other) = delete;
	BaseMockDevice(BaseMockDevice&& other) = delete;
	BaseMockDevice& operator =(const BaseMockDevice& other) = delete;
	BaseMockDevice& operator =(BaseMockDevice&& other) = delete;

	void AttachToV2MPDevice(V2MP_Device* device);
	void DetachFromV2MPDevice();
	bool IsAttachedToDevice() const;

	V2MP_Device* WrappedDevice();
	const V2MP_Device* WrappedDevice() const;

	bool AllocateConnectedMailbox(size_t sizeInBytes);
	bool DeallocateConnectedMailbox();
	bool RelinquishConnectedMailbox();

	size_t DataTransferSpeed() const;
	void SetDataTransferSpeed(size_t bytesPerClockCycle);

protected:
	virtual void OnDeviceAttached();
	virtual void OnDeviceAboutToBeDetached();
	virtual void OnDeviceAboutToBeDestroyed();
	virtual void OnPoll();
	virtual void OnMailboxControlAcquired();

private:
	void Ifc_OnDeviceAboutToBeDestroyed();
	void Ifc_OnPoll();
	void Ifc_OnMailboxControlAcquired();

	void ConstructAndSetCallbacks();

	V2MP_Device* m_Device = nullptr;
};
