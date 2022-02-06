include(win32_export)
create_export_header(${TARGETNAME_V2MP_ASM})

add_library(${TARGETNAME_V2MP_ASM}
	src/Dummy.cpp
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)
target_compile_definitions(${TARGETNAME_V2MP_ASM} PRIVATE "SIZEOF_VOID=${CMAKE_SIZEOF_VOID_P}")
