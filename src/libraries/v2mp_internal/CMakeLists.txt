add_library(${TARGETNAME_V2MP_INTERNAL} STATIC
	include/${TARGETNAME_V2MP_INTERNAL}/Modules/CPU.h
	include/${TARGETNAME_V2MP_INTERNAL}/Modules/Mainboard.h
	include/${TARGETNAME_V2MP_INTERNAL}/Modules/MemoryStore.h
	include/${TARGETNAME_V2MP_INTERNAL}/Modules/Supervisor.h
	include/${TARGETNAME_V2MP_INTERNAL}/Modules/VirtualMachine.h
	include/${TARGETNAME_V2MP_INTERNAL}/Defs.h

	src/Modules/CPU_Instructions.h
	src/Modules/CPU_Instructions.c
	src/Modules/CPU_Internal.h
	src/Modules/CPU_Internal.c
	src/Modules/CPU.c
	src/Modules/Mainboard.c
	src/Modules/MemoryStore.c
	src/Modules/Supervisor_Action_Stack.h
	src/Modules/Supervisor_Action_Stack.c
	src/Modules/Supervisor_Action.h
	src/Modules/Supervisor_Action.c
	src/Modules/Supervisor_CPUInterface.h
	src/Modules/Supervisor_CPUInterface.c
	src/Modules/Supervisor_Internal.h
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
