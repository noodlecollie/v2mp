include(compiler_settings)

add_library(${TARGETNAME_LIBTOOLCHAINCOMPONENTS} STATIC
	include/${TARGETNAME_LIBTOOLCHAINCOMPONENTS}/FilePool.h
	include/${TARGETNAME_LIBTOOLCHAINCOMPONENTS}/InputFile.h
	include/${TARGETNAME_LIBTOOLCHAINCOMPONENTS}/OutputFile.h

	src/FilePool.cpp
	src/InputFile.cpp
	src/OutputFile.cpp
)

target_include_directories(${TARGETNAME_LIBTOOLCHAINCOMPONENTS} PUBLIC include)
target_include_directories(${TARGETNAME_LIBTOOLCHAINCOMPONENTS} PRIVATE src)

set_strict_compile_settings(${TARGETNAME_LIBTOOLCHAINCOMPONENTS})
