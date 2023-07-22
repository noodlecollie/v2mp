add_library(${TARGETNAME_CJSON} STATIC
	include/cJSON.h
	src/cJSON.c
)

target_include_directories(${TARGETNAME_CJSON}
	PUBLIC include
	PRIVATE src
)
