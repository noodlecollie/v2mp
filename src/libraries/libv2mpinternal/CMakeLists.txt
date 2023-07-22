include(compiler_settings)

add_library(${TARGETNAME_LIBV2MPINTERNAL} STATIC
	include/${TARGETNAME_LIBV2MPINTERNAL}/Modules/CPU.h
	include/${TARGETNAME_LIBV2MPINTERNAL}/Modules/Mainboard.h
	include/${TARGETNAME_LIBV2MPINTERNAL}/Modules/MemoryStore.h
	include/${TARGETNAME_LIBV2MPINTERNAL}/Modules/Supervisor.h
	include/${TARGETNAME_LIBV2MPINTERNAL}/Modules/VirtualMachine.h
	include/${TARGETNAME_LIBV2MPINTERNAL}/Defs.h

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

target_include_directories(${TARGETNAME_LIBV2MPINTERNAL} PUBLIC include)
target_include_directories(${TARGETNAME_LIBV2MPINTERNAL} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBV2MPINTERNAL} PUBLIC
	${TARGETNAME_LIBSHAREDCOMPONENTS}
)

target_link_libraries(${TARGETNAME_LIBV2MPINTERNAL} PRIVATE
	${TARGETNAME_LIBBASEUTIL}
)

set_strict_compile_settings(${TARGETNAME_LIBV2MPINTERNAL})
