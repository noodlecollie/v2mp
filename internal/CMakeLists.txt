add_library(${TARGETNAME_V2MP_INTERNAL} STATIC
	src/CircularBuffer.c
	src/HexTree.c
)

target_include_directories(${TARGETNAME_V2MP_INTERNAL} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_INTERNAL} PRIVATE src)
