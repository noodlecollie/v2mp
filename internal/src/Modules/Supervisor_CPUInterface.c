#include "Modules/Supervisor_CPUInterface.h"
#include "Modules/Supervisor_Internal.h"
#include "Modules/Supervisor_Action.h"

V2MP_Word FetchInstructionWord(void* opaqueSv, V2MP_Word address, V2MP_Word* destReg);
void RequestLoadWordFromDS(void* opaqueSv, V2MP_Word address, V2MP_RegisterIndex destReg);
void RequestStoreWordToDS(void* opaqueSv, V2MP_Word address, V2MP_Word wordToStore);
void PerformDevicePortQuery(void* opaqueSv, V2MP_Word port, V2MP_Word queryType);
void RequestDevicePortDirectRead(void* opaqueSv, V2MP_Word port);
void RequestDevicePortIndirectRead(void* opaqueSv, V2MP_Word port, V2MP_Word dsSrcAddress, V2MP_Word dsMaxBytes);
void RequestDevicePortDirectWrite(void* opaqueSv, V2MP_Word port);
void RequestDevicePortIndirectWrite(void* opaqueSv, V2MP_Word port, V2MP_Word dsDestAddress, V2MP_Word dsMaxBytes);
void RequestRelinquishMailbox(void* opaqueSv, V2MP_Word port);
void RequestUsableByteCount(void* opaqueSv, V2MP_Word port);

void V2MP_Supervisor_CreateCPUInterface(V2MP_Supervisor* supervisor, V2MP_CPU_SupervisorInterface* interface)
{
	if ( !supervisor || !interface )
	{
		return;
	}

	interface->supervisor = supervisor;

	interface->fetchInstructionWord = &FetchInstructionWord;
	interface->requestLoadWordFromDS = &RequestLoadWordFromDS;
	interface->requestStoreWordToDS = &RequestStoreWordToDS;
	interface->performDevicePortQuery = &PerformDevicePortQuery;
	interface->requestDevicePortDirectRead = &RequestDevicePortDirectRead;
	interface->requestDevicePortIndirectRead = &RequestDevicePortIndirectRead;
	interface->requestDevicePortDirectWrite = &RequestDevicePortDirectWrite;
	interface->requestDevicePortIndirectWrite = &RequestDevicePortIndirectWrite;
	interface->requestRelinquishMailbox = &RequestRelinquishMailbox;
	interface->requestUsableByteCount = &RequestUsableByteCount;
}

V2MP_Word FetchInstructionWord(void* opaqueSv, V2MP_Word address, V2MP_Word* destReg)
{
	return V2MP_Supervisor_FetchInstructionWord((V2MP_Supervisor*)opaqueSv, address, destReg);
}

void RequestLoadWordFromDS(void* opaqueSv, V2MP_Word address, V2MP_RegisterIndex destReg)
{
	V2MP_Supervisor_RequestLoadWordFromDS((V2MP_Supervisor*)opaqueSv, address, destReg);
}

void RequestStoreWordToDS(void* opaqueSv, V2MP_Word address, V2MP_Word wordToStore)
{
	V2MP_Supervisor_RequestStoreWordToDS((V2MP_Supervisor*)opaqueSv, address, wordToStore);
}

void PerformDevicePortQuery(void* opaqueSv, V2MP_Word port, V2MP_Word queryType)
{
	V2MP_Supervisor_PerformDevicePortQuery((V2MP_Supervisor*)opaqueSv, port, queryType);
}

void RequestDevicePortDirectRead(void* opaqueSv, V2MP_Word port)
{
	V2MP_Supervisor_RequestDevicePortDirectRead((V2MP_Supervisor*)opaqueSv, port);
}

void RequestDevicePortIndirectRead(void* opaqueSv, V2MP_Word port, V2MP_Word dsDestAddress, V2MP_Word dsMaxBytes)
{
	V2MP_Supervisor_RequestDevicePortIndirectRead((V2MP_Supervisor*)opaqueSv, port, dsDestAddress, dsMaxBytes);
}

void RequestDevicePortDirectWrite(void* opaqueSv, V2MP_Word port)
{
	V2MP_Supervisor_RequestDevicePortDirectWrite((V2MP_Supervisor*)opaqueSv, port);
}

void RequestDevicePortIndirectWrite(void* opaqueSv, V2MP_Word port, V2MP_Word dsSrcAddress, V2MP_Word dsMaxBytes)
{
	V2MP_Supervisor_RequestDevicePortIndirectWrite((V2MP_Supervisor*)opaqueSv, port, dsSrcAddress, dsMaxBytes);
}

void RequestRelinquishMailbox(void* opaqueSv, V2MP_Word port)
{
	V2MP_Supervisor_RequestRelinquishMailbox((V2MP_Supervisor*)opaqueSv, port);
}

void RequestUsableByteCount(void* opaqueSv, V2MP_Word port)
{
	V2MP_Supervisor_RequestUsableByteCount((V2MP_Supervisor*)opaqueSv, port);
}
