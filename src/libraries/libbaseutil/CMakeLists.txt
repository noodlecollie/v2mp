add_library(${TARGETNAME_LIBBASEUTIL} STATIC
	src/Heap.c
	src/String.c
	src/Util.c
)

target_include_directories(${TARGETNAME_LIBBASEUTIL} PUBLIC include)
target_include_directories(${TARGETNAME_LIBBASEUTIL} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBBASEUTIL} PUBLIC
	${TARGETNAME_UTHASH}
)
