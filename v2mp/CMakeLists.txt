include(win32_export)
create_export_header(${TARGETNAME_V2MP})

set(PUBLIC_HEADERS_GENERAL
	include/${TARGETNAME_V2MP}/CPU.h
	include/${TARGETNAME_V2MP}/Defs.h
	include/${TARGETNAME_V2MP}/LibExport.h
	include/${TARGETNAME_V2MP}/MemoryStore.h
)

set(PUBLIC_HEADERS_ALL
	${PUBLIC_HEADERS_GENERAL}
)

set(SOURCES_ALL
	src/CPU.c
	src/DevicePort.c
	src/DevicePortStore.c
	src/MemoryStore.c
)

if(BUILD_SHARED_LIBS)
	add_library(${TARGETNAME_V2MP} SHARED ${PUBLIC_HEADERS_ALL} ${SOURCES_ALL})
else()
	add_library(${TARGETNAME_V2MP} STATIC ${PUBLIC_HEADERS_ALL} ${SOURCES_ALL})
endif()

target_include_directories(${TARGETNAME_V2MP} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP} PRIVATE src)

target_link_libraries(${TARGETNAME_V2MP} PRIVATE ${TARGETNAME_V2MP_COMPONENTS})

install(TARGETS ${TARGETNAME_V2MP}
	ARCHIVE DESTINATION ${TARGETNAME_V2MP}/lib
	LIBRARY DESTINATION ${TARGETNAME_V2MP}/lib
	RUNTIME DESTINATION ${TARGETNAME_V2MP}/bin
)

install(FILES ${PUBLIC_HEADERS_GENERAL} DESTINATION ${TARGETNAME_V2MP}/include/${TARGETNAME_V2MP})

add_export_defs(${TARGETNAME_V2MP})
