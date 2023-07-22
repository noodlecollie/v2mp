include(compiler_settings)

add_executable(${TARGETNAME_V2MPEXPLORER}
	src/Main.cpp
)

target_include_directories(${TARGETNAME_V2MPEXPLORER} PRIVATE
	src
)

target_link_libraries(${TARGETNAME_V2MPEXPLORER} PRIVATE
	${TARGETNAME_RAYLIB}
)

set_strict_compile_settings(${TARGETNAME_V2MPEXPLORER})

install(TARGETS ${TARGETNAME_V2MPEXPLORER})
