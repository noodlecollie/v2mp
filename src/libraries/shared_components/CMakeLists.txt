add_library(${TARGETNAME_SHARED_COMPONENTS} STATIC
	src/CircularBuffer.c
	src/DoubleLinkedList.c
	src/HexTree.c
)

target_include_directories(${TARGETNAME_SHARED_COMPONENTS} PUBLIC include)
target_include_directories(${TARGETNAME_SHARED_COMPONENTS} PRIVATE src)

target_link_libraries(${TARGETNAME_SHARED_COMPONENTS} PRIVATE
	${TARGETNAME_BASEUTIL}
)
