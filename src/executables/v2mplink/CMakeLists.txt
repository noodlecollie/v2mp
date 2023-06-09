configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/Version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Version.gen.h"
)

add_executable(${TARGETNAME_V2MPLINK}
	src/CommandLineArgs.h
	src/Main.cpp
	src/V2MPLinkArgumentParser.h
	src/V2MPLinkArgumentParser.cpp
	src/Version.gen.h
)

target_include_directories(${TARGETNAME_V2MPLINK} PRIVATE
	src
)

target_link_libraries(${TARGETNAME_V2MPLINK} PRIVATE
	${TARGETNAME_LIBV2MPLINK}
	${TARGETNAME_ARGPARSE}
)

install(TARGETS ${TARGETNAME_V2MPLINK})
