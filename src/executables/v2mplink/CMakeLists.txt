add_executable(${TARGETNAME_V2MPLINK}
	src/Main.cpp
)

target_include_directories(${TARGETNAME_V2MPLINK} PRIVATE
	src
)

target_link_libraries(${TARGETNAME_V2MPLINK} PRIVATE
	${TARGETNAME_LIBV2MPLINK}
	${TARGETNAME_ARGPARSE}
)

install(TARGETS ${TARGETNAME_V2MPLINK})
