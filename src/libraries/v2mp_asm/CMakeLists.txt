include(win32_export)
create_export_header(${TARGETNAME_V2MP_ASM})

add_library(${TARGETNAME_V2MP_ASM}
	src/InputFile.c
	src/ParseContext.c
	src/TempTest.c
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)

target_link_libraries(${TARGETNAME_V2MP_ASM} PRIVATE
	${TARGETNAME_BASEUTIL}
	cwalk
)
