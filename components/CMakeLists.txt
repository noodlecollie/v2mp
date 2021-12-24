add_library(${TARGETNAME_V2MP_COMPONENTS} STATIC
	src/CircularBuffer.c
	src/HexTree.c
)

target_include_directories(${TARGETNAME_V2MP_COMPONENTS} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_COMPONENTS} PRIVATE src)
