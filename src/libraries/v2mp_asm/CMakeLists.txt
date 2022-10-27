configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/LibExport.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGETNAME_V2MP_ASM}/LibExport.h"
)

add_library(${TARGETNAME_V2MP_ASM}
	include/${TARGETNAME_V2MP_ASM}/Assembler.h
	include/${TARGETNAME_V2MP_ASM}/Exception.h

	src/Assembler/Assembler.h
	src/Assembler/Assembler.cpp
	src/Exceptions/AssemblerException.h
	src/Exceptions/AssemblerException.cpp
	src/Exceptions/PublicException.cpp
	src/Exceptions/PublicExceptionIDs.h
	src/Exceptions/PublicExceptionIDs.cpp
	src/Files/FilePool.h
	src/Files/FilePool.cpp
	src/Files/InputFile.h
	src/Files/InputFile.cpp
	src/Files/InputReader.h
	src/Files/InputReader.cpp
	src/Utils/StringUtils.cpp
	src/Parser/Parser.h
	src/Parser/Parser.cpp
	src/Parser/Tokeniser.h
	src/Parser/Tokeniser.cpp
	src/Interface_Assembler.h
	src/Interface_Assembler.cpp
	src/Interface_Exception.h
	src/Interface_Exception.cpp
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)

target_link_libraries(${TARGETNAME_V2MP_ASM} PRIVATE
	${TARGETNAME_BASEUTIL}
	${TARGETNAME_SHARED_COMPONENTS}
	cwalk
)

target_compile_definitions(${TARGETNAME_V2MP_ASM} PRIVATE "V2MPASM_PRODUCER")
