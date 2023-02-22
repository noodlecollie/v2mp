add_library(${TARGETNAME_LIBBASEUTIL} STATIC
	include/${TARGETNAME_LIBBASEUTIL}/Array.h
	include/${TARGETNAME_LIBBASEUTIL}/Filesystem.h
	include/${TARGETNAME_LIBBASEUTIL}/Heap.h
	include/${TARGETNAME_LIBBASEUTIL}/String.h
	include/${TARGETNAME_LIBBASEUTIL}/UTHash_V2MP.h
	include/${TARGETNAME_LIBBASEUTIL}/Util.h
	src/Heap.c
	src/String.c
	src/Util.c
)

target_include_directories(${TARGETNAME_LIBBASEUTIL} PUBLIC include)
target_include_directories(${TARGETNAME_LIBBASEUTIL} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBBASEUTIL} PUBLIC
	${TARGETNAME_UTHASH}
)
