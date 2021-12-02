include(win32_export)
create_export_header(V2MP)

set(PUBLIC_HEADERS_GENERAL
	include/V2MP/CPU.h
	include/V2MP/Types.h
)

set(PUBLIC_HEADERS_ALL
	${PUBLIC_HEADERS_GENERAL}
)

set(SOURCES_ALL
	src/CPU.c
)

if(BUILD_SHARED_LIBS)
	add_library(V2MP SHARED ${PUBLIC_HEADERS_ALL} ${SOURCES_ALL})
else()
	add_library(V2MP STATIC ${PUBLIC_HEADERS_ALL} ${SOURCES_ALL})
endif()

target_include_directories(V2MP PUBLIC include)
target_include_directories(V2MP PRIVATE src)

install(TARGETS V2MP
	ARCHIVE DESTINATION V2MP/lib
	LIBRARY DESTINATION V2MP/lib
	RUNTIME DESTINATION V2MP/bin
)

install(FILES ${PUBLIC_HEADERS_GENERAL} DESTINATION V2MP/include/V2MP)

add_export_defs(V2MP)
