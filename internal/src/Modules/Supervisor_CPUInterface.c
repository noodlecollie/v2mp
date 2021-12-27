#include "Modules/Supervisor_CPUInterface.h"
#include "Modules/Supervisor_Internal.h"

V2MP_Word FetchInstructionWord(void* supervisor, V2MP_Word address, V2MP_Word* destReg);
void RequestLoadWordFromDS(void* supervisor, V2MP_Word address, V2MP_Word* destReg);
void RequestStoreWordToDS(void* supervisor, V2MP_Word address, V2MP_Word wordToStore);

void V2MP_Supervisor_CreateCPUInterface(V2MP_Supervisor* supervisor, V2MP_CPURenameMe_SupervisorInterface* interface)
{
	if ( !supervisor || !interface )
	{
		return;
	}

	interface->supervisor = supervisor;

	interface->fetchInstructionWord = &FetchInstructionWord;
	interface->requestLoadWordFromDS = &RequestLoadWordFromDS;
	interface->requestStoreWordToDS = &RequestStoreWordToDS;
}

V2MP_Word FetchInstructionWord(void* supervisor, V2MP_Word address, V2MP_Word* destReg)
{
	return V2MP_Supervisor_FetchInstructionWord((V2MP_Supervisor*)supervisor, address, destReg);
}

void RequestLoadWordFromDS(void* supervisor, V2MP_Word address, V2MP_Word* destReg)
{
	// TODO
	(void)supervisor;
	(void)address;
	(void)destReg;
}

void RequestStoreWordToDS(void* supervisor, V2MP_Word address, V2MP_Word wordToStore)
{
	// TODO
	(void)supervisor;
	(void)address;
	(void)wordToStore;
}
