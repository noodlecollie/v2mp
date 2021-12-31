#include <cstring>
#include "Helpers/MockDevice.h"

MockDevice::MockDevice()
{
}

MockDevice::~MockDevice()
{
	DetachFromV2MPDevice();
}

void MockDevice::AttachToV2MPDevice(V2MP_Device* device)
{
	DetachFromV2MPDevice();

	m_Device = device;
	ConstructAndSetExternalInterface();
}

void MockDevice::DetachFromV2MPDevice()
{
	if ( m_Device )
	{
		V2MP_Device_ClearExtInterface(m_Device);
		m_Device = nullptr;
	}
}

bool MockDevice::IsAttachedToDevice() const
{
	return m_Device != nullptr;
}

void MockDevice::Ifc_OnDeviceAboutToBeDestroyed()
{
	// Should never happen anyway:
	if ( !m_Device )
	{
		return;
	}

	OnDeviceAboutToBeDestroyed();
	m_Device = nullptr;
}

V2MP_Device* MockDevice::WrappedDevice()
{
	return m_Device;
}

const V2MP_Device* MockDevice::WrappedDevice() const
{
	return m_Device;
}

void MockDevice::Ifc_OnPoll()
{
	if ( !m_Device )
	{
		return;
	}

	OnPoll();
}

void MockDevice::ConstructAndSetExternalInterface()
{
	if ( !m_Device )
	{
		return;
	}

	V2MP_Device_ExtInterface interface;
	memset(&interface, 0, sizeof(interface));

	interface.userData = reinterpret_cast<void*>(this);

	interface.onDeviceAboutToBeDestroyed = [](void* userData, V2MP_Device*)
	{
		reinterpret_cast<MockDevice*>(userData)->Ifc_OnDeviceAboutToBeDestroyed();
	};

	interface.onPoll = [](void* userData, V2MP_Device*)
	{
		reinterpret_cast<MockDevice*>(userData)->Ifc_OnPoll();
	};

	V2MP_Device_SetExtInterface(m_Device, &interface);
}

void MockDevice::OnDeviceAboutToBeDestroyed()
{
	// Default implementation does nothing.
}

void MockDevice::OnPoll()
{
	// Default implementation does nothing.
}
