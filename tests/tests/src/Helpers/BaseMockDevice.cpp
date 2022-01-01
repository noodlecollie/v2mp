#include <cstring>
#include "Helpers/BaseMockDevice.h"

BaseMockDevice::BaseMockDevice()
{
}

BaseMockDevice::~BaseMockDevice()
{
	DetachFromV2MPDevice();
}

void BaseMockDevice::AttachToV2MPDevice(V2MP_Device* device)
{
	DetachFromV2MPDevice();

	m_Device = device;

	if ( !m_Device )
	{
		return;
	}

	ConstructAndSetExternalInterface();
	OnDeviceAttached();
}

void BaseMockDevice::DetachFromV2MPDevice()
{
	if ( m_Device )
	{
		OnDeviceAboutToBeDetached();
		V2MP_Device_ClearExtInterface(m_Device);
		m_Device = nullptr;
	}
}

bool BaseMockDevice::IsAttachedToDevice() const
{
	return m_Device != nullptr;
}

V2MP_Device* BaseMockDevice::WrappedDevice()
{
	return m_Device;
}

const V2MP_Device* BaseMockDevice::WrappedDevice() const
{
	return m_Device;
}

bool BaseMockDevice::AllocateMailbox(size_t sizeInBytes)
{
	return V2MP_Device_AllocateConnectedMailbox(m_Device, sizeInBytes);
}

bool BaseMockDevice::DeallocateMailbox()
{
	return V2MP_Device_DeallocateConnectedMailbox(m_Device);
}

void BaseMockDevice::RelinquishMailbox()
{
	V2MP_Device_RelinquishConnectedMailbox(m_Device);
}

void BaseMockDevice::Ifc_OnDeviceAboutToBeDestroyed()
{
	// Should never happen anyway:
	if ( !m_Device )
	{
		return;
	}

	OnDeviceAboutToBeDestroyed();
	m_Device = nullptr;
}

void BaseMockDevice::Ifc_OnMailboxControlAcquired()
{
	OnMailboxControlAcquired();
}

void BaseMockDevice::Ifc_OnPoll()
{
	if ( !m_Device )
	{
		return;
	}

	OnPoll();
}

void BaseMockDevice::ConstructAndSetExternalInterface()
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
		reinterpret_cast<BaseMockDevice*>(userData)->Ifc_OnDeviceAboutToBeDestroyed();
	};

	interface.onPoll = [](void* userData, V2MP_Device*)
	{
		reinterpret_cast<BaseMockDevice*>(userData)->Ifc_OnPoll();
	};

	interface.onMailboxControlAcquired = [](void* userData, V2MP_Device*)
	{
		reinterpret_cast<BaseMockDevice*>(userData)->Ifc_OnMailboxControlAcquired();
	};

	V2MP_Device_SetExtInterface(m_Device, &interface);
}

void BaseMockDevice::OnDeviceAttached()
{
	// Default implementation does nothing.
}

void BaseMockDevice::OnDeviceAboutToBeDetached()
{
	// Default implementation does nothing.
}

void BaseMockDevice::OnDeviceAboutToBeDestroyed()
{
	// Default implementation does nothing.
}

void BaseMockDevice::OnPoll()
{
	// Default implementation does nothing.
}

void BaseMockDevice::OnMailboxControlAcquired()
{
	// Default implementation does nothing.
}
