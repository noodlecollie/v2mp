add_executable(${TARGETNAME_V2MPASM}
	src/Main.c
)

target_include_directories(${TARGETNAME_V2MPASM} PRIVATE src)
target_link_libraries(${TARGETNAME_V2MPASM} PRIVATE ${TARGETNAME_LIBV2MPASM})

install(TARGETS ${TARGETNAME_V2MPASM})
