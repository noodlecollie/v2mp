add_library(${TARGETNAME_RAYGUI} STATIC
	include/raygui.h
	src/raygui.c
)

target_include_directories(${TARGETNAME_RAYGUI}
	PUBLIC include
	PRIVATE src
)

target_link_libraries(${TARGETNAME_RAYGUI}
	PUBLIC ${TARGETNAME_RAYLIB}
)
