add_library(${TARGETNAME_LIBSHAREDCOMPONENTS} STATIC
	include/${TARGETNAME_LIBSHAREDCOMPONENTS}/CircularBuffer.h
	include/${TARGETNAME_LIBSHAREDCOMPONENTS}/DoubleLinkedList.h
	include/${TARGETNAME_LIBSHAREDCOMPONENTS}/HexTree.h

	src/CircularBuffer.c
	src/DoubleLinkedList.c
	src/HexTree.c
)

target_include_directories(${TARGETNAME_LIBSHAREDCOMPONENTS} PUBLIC include)
target_include_directories(${TARGETNAME_LIBSHAREDCOMPONENTS} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBSHAREDCOMPONENTS} PRIVATE
	${TARGETNAME_LIBBASEUTIL}
)
