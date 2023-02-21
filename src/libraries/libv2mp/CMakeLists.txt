configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/LibExport.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGETNAME_LIBV2MP}/LibExport.gen.h"
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/Interface_Version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Interface_Version.gen.h"
)

set(PUBLIC_HEADERS_ALL
	include/${TARGETNAME_LIBV2MP}/Dummy.h
	include/${TARGETNAME_LIBV2MP}/LibExport.gen.h
	include/${TARGETNAME_LIBV2MP}/Version.h
)

set(SOURCES_ALL
	src/Dummy.c
	src/Interface_Version.c
)

add_library(${TARGETNAME_LIBV2MP} SHARED ${PUBLIC_HEADERS_ALL} ${SOURCES_ALL})

target_include_directories(${TARGETNAME_LIBV2MP} PUBLIC include)
target_include_directories(${TARGETNAME_LIBV2MP} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBV2MP} PRIVATE ${TARGETNAME_LIBV2MPINTERNAL})

target_compile_definitions(${TARGETNAME_LIBV2MP} PRIVATE "V2MP_PRODUCER")

install(TARGETS ${TARGETNAME_LIBV2MP})
install(FILES ${PUBLIC_HEADERS_ALL} DESTINATION include/${TARGETNAME_LIBV2MP})
