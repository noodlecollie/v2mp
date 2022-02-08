add_library(${TARGETNAME_BASEUTIL} STATIC
	src/Heap.c
	src/String.c
)

target_include_directories(${TARGETNAME_BASEUTIL} PUBLIC include)
target_include_directories(${TARGETNAME_BASEUTIL} PRIVATE src)
