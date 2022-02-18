configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/LibExport.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGETNAME_V2MP_ASM}/LibExport.h"
)

add_library(${TARGETNAME_V2MP_ASM}
	src/CodewordDescriptors/CWD_Base.c
	src/CodewordDescriptors/CWD_Factory.c
	src/CodewordDescriptors/CWD_Instruction.c
	src/CodewordDescriptors/CWDList.c

	src/Instructions/InstructionMeta_Add.c
	src/Instructions/InstructionMeta.c

	src/Tokens/TokenMeta_Label.c
	src/Tokens/TokenMeta_LineComment.c
	src/Tokens/TokenMeta_MultilineComment.c
	src/Tokens/TokenMeta_Name.c
	src/Tokens/TokenMeta_NumericLiteral.c
	src/Tokens/TokenMeta_Preprocessor.c
	src/Tokens/TokenMeta.c

	src/InputFile.c
	src/ParseContext.c
	src/ParseException_Internal.c
	src/ParseException.c
	src/Parser_Routines.c
	src/Parser.c
	src/TempTest.c
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)

target_link_libraries(${TARGETNAME_V2MP_ASM} PRIVATE
	${TARGETNAME_BASEUTIL}
	${TARGETNAME_SHARED_COMPONENTS}
	cwalk
)

target_compile_definitions(${TARGETNAME_V2MP_ASM} PRIVATE "V2MPASM_PRODUCER")
