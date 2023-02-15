add_executable(${TARGETNAME_ASSEMBLER}
	src/Main.c
)

target_include_directories(${TARGETNAME_ASSEMBLER} PRIVATE src)
target_link_libraries(${TARGETNAME_ASSEMBLER} PRIVATE ${TARGETNAME_LIBV2MPASM})
