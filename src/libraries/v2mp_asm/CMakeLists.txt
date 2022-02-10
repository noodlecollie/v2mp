include(win32_export)
create_export_header(${TARGETNAME_V2MP_ASM})

add_library(${TARGETNAME_V2MP_ASM}
	src/Tokens/TokenMeta_Label.c
	src/Tokens/TokenMeta_LineComment.c
	src/Tokens/TokenMeta_MultilineComment.c
	src/Tokens/TokenMeta_Name.c
	src/Tokens/TokenMeta_NumericLiteral.c
	src/Tokens/TokenMeta_Preprocessor.c
	src/Tokens/TokenMeta.c

	src/InputFile.c
	src/ParseContext.c
	src/TempTest.c
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)

target_link_libraries(${TARGETNAME_V2MP_ASM} PRIVATE
	${TARGETNAME_BASEUTIL}
	cwalk
)
