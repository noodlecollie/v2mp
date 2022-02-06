add_library(${TARGETNAME_V2MP_ASM}
	src/AsmException_Internal.c
	src/AsmException.c
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)
target_link_libraries(${TARGETNAME_V2MP_ASM} ${TARGETNAME_V2MP_INTERNAL})
