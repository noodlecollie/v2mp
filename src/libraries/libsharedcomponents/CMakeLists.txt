add_library(${TARGETNAME_LIBSHAREDCOMPONENTS} STATIC
	src/CircularBuffer.c
	src/DoubleLinkedList.c
	src/HexTree.c
)

target_include_directories(${TARGETNAME_LIBSHAREDCOMPONENTS} PUBLIC include)
target_include_directories(${TARGETNAME_LIBSHAREDCOMPONENTS} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBSHAREDCOMPONENTS} PRIVATE
	${TARGETNAME_LIBBASEUTIL}
)
