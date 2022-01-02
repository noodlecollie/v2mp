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

	ConstructAndSetCallbacks();
	OnDeviceAttached();
}

void BaseMockDevice::DetachFromV2MPDevice()
{
	if ( m_Device )
	{
		OnDeviceAboutToBeDetached();
		V2MP_Device_ClearCallbacks(m_Device);
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

size_t BaseMockDevice::DataTransferSpeed() const
{
	return V2MP_Device_GetDataTransferSpeed(WrappedDevice());
}

void BaseMockDevice::SetDataTransferSpeed(size_t bytesPerClockCycle)
{
	V2MP_Device_SetDataTransferSpeed(WrappedDevice(), bytesPerClockCycle);
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

void BaseMockDevice::ConstructAndSetCallbacks()
{
	if ( !m_Device )
	{
		return;
	}

	V2MP_Device_Callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));

	callbacks.userData = reinterpret_cast<void*>(this);

	callbacks.onDeviceAboutToBeDestroyed = [](void* userData, V2MP_Device*)
	{
		reinterpret_cast<BaseMockDevice*>(userData)->Ifc_OnDeviceAboutToBeDestroyed();
	};

	callbacks.onPoll = [](void* userData, V2MP_Device*)
	{
		reinterpret_cast<BaseMockDevice*>(userData)->Ifc_OnPoll();
	};

	callbacks.onMailboxControlAcquired = [](void* userData, V2MP_Device*)
	{
		reinterpret_cast<BaseMockDevice*>(userData)->Ifc_OnMailboxControlAcquired();
	};

	V2MP_Device_SetCallbacks(m_Device, &callbacks);
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
