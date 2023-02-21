configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/Version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Version.gen.h"
)

add_executable(${TARGETNAME_V2MPASM}
	src/CommandLineArgs.h
	src/Main.cpp
	src/V2MPAsmArgumentParser.h
	src/V2MPAsmArgumentParser.cpp
	src/Version.gen.h
)

target_include_directories(${TARGETNAME_V2MPASM} PRIVATE
	src
)

target_link_libraries(${TARGETNAME_V2MPASM} PRIVATE
	${TARGETNAME_LIBV2MPASM}
	${TARGETNAME_ARGPARSE}
)

install(TARGETS ${TARGETNAME_V2MPASM})
