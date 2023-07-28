include(compiler_settings)
include(shared_lib_utils)

generate_export_header_template(${TARGETNAME_LIBV2MP} "${CMAKE_CURRENT_SOURCE_DIR}/configure/LibExport.h.gen.in")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/LibExport.h.gen.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGETNAME_LIBV2MP}/LibExport.gen.h"
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/Interface_Version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Interface_Version.gen.h"
)

set(PUBLIC_HEADERS_ALL
	include/${TARGETNAME_LIBV2MP}/Modules/CPU.h
	include/${TARGETNAME_LIBV2MP}/Modules/Mainboard.h
	include/${TARGETNAME_LIBV2MP}/Modules/MemoryStore.h
	include/${TARGETNAME_LIBV2MP}/Modules/Supervisor.h
	include/${TARGETNAME_LIBV2MP}/Modules/VirtualMachine.h
	include/${TARGETNAME_LIBV2MP}/Defs.h
	include/${TARGETNAME_LIBV2MP}/LibExport.gen.h
	include/${TARGETNAME_LIBV2MP}/Version.h
)

set(SOURCES_ALL
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
	src/Interface_Version.gen.h
	src/Interface_Version.c
)

add_library(${TARGETNAME_LIBV2MP} SHARED
	${PUBLIC_HEADERS_ALL}
	${SOURCES_ALL}
)

target_include_directories(${TARGETNAME_LIBV2MP} PUBLIC include)
target_include_directories(${TARGETNAME_LIBV2MP} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBV2MP} PRIVATE
	${TARGETNAME_LIBBASEUTIL}
	${TARGETNAME_LIBSHAREDCOMPONENTS}
)

set_strict_compile_settings(${TARGETNAME_LIBV2MP})

install(TARGETS ${TARGETNAME_LIBV2MP})
install(FILES ${PUBLIC_HEADERS_ALL} DESTINATION include/${TARGETNAME_LIBV2MP})
