add_library(${TARGETNAME_V2MP_ASM}
	src/AsmException.c
	src/Assembler.c
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)
target_link_libraries(${TARGETNAME_V2MP_ASM} ${TARGETNAME_V2MP_INTERNAL})
