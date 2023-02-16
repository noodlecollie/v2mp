add_executable(${TARGETNAME_V2MPLINK}
	src/Main.cpp
)

target_include_directories(${TARGETNAME_V2MPLINK} PRIVATE
	src
)

install(TARGETS ${TARGETNAME_V2MPLINK})
