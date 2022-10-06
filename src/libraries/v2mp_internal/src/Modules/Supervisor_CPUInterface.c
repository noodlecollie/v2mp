#include "Modules/Supervisor_CPUInterface.h"
#include "Modules/Supervisor_Internal.h"
#include "Modules/Supervisor_Action.h"

V2MP_Word FetchInstructionWord(void* opaqueSv, V2MP_Word address, V2MP_Word* destReg);
void RequestLoadWordFromDS(void* opaqueSv, V2MP_Word address, V2MP_RegisterIndex destReg);
void RequestStoreWordToDS(void* opaqueSv, V2MP_Word address, V2MP_Word wordToStore);
void RequestStackPush(void* opaqueSv, V2MP_Word regFlags);
void RequestStackPop(void* opaqueSv, V2MP_Word regFlags);

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
	interface->requestStackPush = &RequestStackPush;
	interface->requestStackPop = &RequestStackPop;
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

void RequestStackPush(void* opaqueSv, V2MP_Word regFlags)
{
	V2MP_Supervisor_RequestStackPush((V2MP_Supervisor*)opaqueSv, regFlags);
}

void RequestStackPop(void* opaqueSv, V2MP_Word regFlags)
{
	V2MP_Supervisor_RequestStackPop((V2MP_Supervisor*)opaqueSv, regFlags);
}
