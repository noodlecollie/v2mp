#ifndef V2MP_MODULES_DEVICE_INTERNAL_H
#define V2MP_MODULES_DEVICE_INTERNAL_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/Device.h"
#include "SharedComponents/DoubleLinkedList.h"

struct V2MP_DevicePort;

struct V2MP_Device
{
	V2MPSC_DoubleLL_Node* ownerNode;
	struct V2MP_DevicePort* connectedPort;
	V2MP_Device_Callbacks callbacks;
	size_t dataTransferSpeed;
};

V2MP_Device* V2MP_Device_AllocateAndInit(void);
void V2MP_Device_DeinitAndFree(V2MP_Device* device);

void V2MP_Device_NotifyConnectedToPort(V2MP_Device* device, struct V2MP_DevicePort* port);
void V2MP_Device_NotifyDisconnectedFromPort(V2MP_Device* device);
bool V2MP_Device_NotifyMailboxReadyForInteraction(V2MP_Device* device);

V2MPSC_DoubleLL_Node* V2MP_Device_GetOwnerNode(const V2MP_Device* device);
void V2MP_Device_SetOwnerNode(V2MP_Device* device, V2MPSC_DoubleLL_Node* node);

bool V2MP_Device_Poll(V2MP_Device* device);

#endif // V2MP_MODULES_DEVICE_INTERNAL_H
