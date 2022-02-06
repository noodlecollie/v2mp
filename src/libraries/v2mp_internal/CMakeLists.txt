add_library(${TARGETNAME_V2MP_INTERNAL} STATIC
	src/Components/CircularBuffer.c
	src/Components/DoubleLinkedList.c
	src/Components/HexTree.c

	src/Modules/CPU_DevicePortOperations.c
	src/Modules/CPU_Instructions.c
	src/Modules/CPU_Internal.c
	src/Modules/CPU.c
	src/Modules/Device_Internal.c
	src/Modules/Device.c
	src/Modules/DeviceCollection.c
	src/Modules/DevicePort_Internal.c
	src/Modules/DevicePort.c
	src/Modules/DevicePortCollection.c
	src/Modules/Mainboard.c
	src/Modules/MemoryStore.c
	src/Modules/Supervisor_Action.c
	src/Modules/Supervisor_Action_DPQ.c
	src/Modules/Supervisor_Action_Stack.c
	src/Modules/Supervisor_CPUInterface.c
	src/Modules/Supervisor_Internal.c
	src/Modules/Supervisor.c
	src/Modules/VirtualMachine.c

	src/Util/Heap.c
)

target_include_directories(${TARGETNAME_V2MP_INTERNAL} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_INTERNAL} PRIVATE src)
