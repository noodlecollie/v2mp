include(compiler_settings)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/LibExport.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGETNAME_LIBV2MPLINK}/LibExport.gen.h"
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/Interface_Version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Interface_Version.gen.h"
)

set(PUBLIC_HEADERS_ALL
	include/${TARGETNAME_LIBV2MPLINK}/LibExport.gen.h
	include/${TARGETNAME_LIBV2MPLINK}/Linker.h
)

set(SOURCES_ALL
	src/Exceptions/LinkerException.h
	src/Exceptions/LinkerException.cpp
	src/Exceptions/PublicException.h
	src/Exceptions/PublicException.cpp
	src/Exceptions/PublicExceptionIDs.h
	src/Exceptions/PublicExceptionIDs.cpp
	src/Linker/Linker.h
	src/Linker/Linker.cpp
	src/Interface_Exception.h
	src/Interface_Exception.cpp
	src/Interface_Linker.h
	src/Interface_Linker.cpp
	src/Interface_Version.gen.h
	src/Interface_Version.cpp
)

add_library(${TARGETNAME_LIBV2MPLINK} SHARED
	${PUBLIC_HEADERS_ALL}
	${SOURCES_ALL}
)

target_include_directories(${TARGETNAME_LIBV2MPLINK} PUBLIC include)
target_include_directories(${TARGETNAME_LIBV2MPLINK} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBV2MPLINK} PRIVATE
	${TARGETNAME_LIBBASEUTIL}
	${TARGETNAME_LIBTOOLCHAINCOMPONENTS}
	${TARGETNAME_CJSON}
)

target_compile_definitions(${TARGETNAME_LIBV2MPLINK} PRIVATE "V2MPLINK_PRODUCER")

set_strict_compile_settings(${TARGETNAME_LIBV2MPLINK})

install(TARGETS ${TARGETNAME_LIBV2MPLINK})
install(FILES ${PUBLIC_HEADERS_ALL} DESTINATION include/${TARGETNAME_LIBV2MPLINK})
