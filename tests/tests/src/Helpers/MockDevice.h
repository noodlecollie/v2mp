#pragma once

#include "V2MPInternal/Modules/Device.h"

class MockDevice
{
public:
	MockDevice();
	~MockDevice();

	MockDevice(const MockDevice& other) = delete;
	MockDevice(MockDevice&& other) = delete;
	MockDevice& operator =(const MockDevice& other) = delete;
	MockDevice& operator =(MockDevice&& other) = delete;

	void AttachToV2MPDevice(V2MP_Device* device);
	void DetachFromV2MPDevice();
	bool IsAttachedToDevice() const;

	V2MP_Device* WrappedDevice();
	const V2MP_Device* WrappedDevice() const;

protected:
	virtual void OnDeviceAboutToBeDestroyed();
	virtual void OnPoll();

private:
	void Ifc_OnDeviceAboutToBeDestroyed();
	void Ifc_OnPoll();

	void ConstructAndSetExternalInterface();

	V2MP_Device* m_Device = nullptr;
};
