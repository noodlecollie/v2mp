add_library(${TARGETNAME_V2MP_INTERNAL} STATIC
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
)

target_include_directories(${TARGETNAME_V2MP_INTERNAL} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_INTERNAL} PRIVATE src)

target_link_libraries(${TARGETNAME_V2MP_INTERNAL} PUBLIC
	${TARGETNAME_SHARED_COMPONENTS}
)

target_link_libraries(${TARGETNAME_V2MP_INTERNAL} PRIVATE
	${TARGETNAME_BASEUTIL}
)
