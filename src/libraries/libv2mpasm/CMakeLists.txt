configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/LibExport.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGETNAME_LIBV2MPASM}/LibExport.h"
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/configure/Interface_Version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Interface_Version.h"
)

set(PUBLIC_HEADERS_ALL
	include/${TARGETNAME_LIBV2MPASM}/Assembler.h
	include/${TARGETNAME_LIBV2MPASM}/Exception.h
	include/${TARGETNAME_LIBV2MPASM}/LibExport.h
	include/${TARGETNAME_LIBV2MPASM}/Version.h
)

set(SOURCES_ALL
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
	src/Files/OutputFile.h
	src/Files/OutputFile.cpp
	src/Utils/StringUtils.cpp
	src/Parser/Parser.h
	src/Parser/Parser.cpp
	src/Parser/Tokeniser.h
	src/Parser/Tokeniser.cpp
	src/ProgramModel/Validators/AsgnCodeWordValidator.h
	src/ProgramModel/Validators/AsgnCodeWordValidator.cpp
	src/ProgramModel/Validators/BasicCodeWordValidator.h
	src/ProgramModel/Validators/BasicCodeWordValidator.cpp
	src/ProgramModel/Validators/BitwCodeWordValidator.h
	src/ProgramModel/Validators/BitwCodeWordValidator.cpp
	src/ProgramModel/Validators/CbxCodeWordValidator.h
	src/ProgramModel/Validators/CbxCodeWordValidator.cpp
	src/ProgramModel/Validators/CodeWordValidatorFactory.h
	src/ProgramModel/Validators/CodeWordValidatorFactory.cpp
	src/ProgramModel/Validators/LdstCodeWordValidator.h
	src/ProgramModel/Validators/LdstCodeWordValidator.cpp
	src/ProgramModel/Validators/MulDivCodeWordValidator.h
	src/ProgramModel/Validators/MulDivCodeWordValidator.cpp
	src/ProgramModel/Validators/SimpleRegAndValueCodeWordValidator.h
	src/ProgramModel/Validators/SimpleRegAndValueCodeWordValidator.cpp
	src/ProgramModel/Validators/StkCodeWordValidator.h
	src/ProgramModel/Validators/StkCodeWordValidator.cpp
	src/ProgramModel/CodeWord.h
	src/ProgramModel/CodeWord.cpp
	src/ProgramModel/CodeWordArg.h
	src/ProgramModel/CodeWordArg.cpp
	src/ProgramModel/CodeWordOutput.h
	src/ProgramModel/CodeWordOutput.cpp
	src/ProgramModel/CodeWordValidation.h
	src/ProgramModel/CodeWordValidation.cpp
	src/ProgramModel/InstructionMeta.h
	src/ProgramModel/InstructionMeta.cpp
	src/ProgramModel/LabelReference.h
	src/ProgramModel/LabelReference.cpp
	src/ProgramModel/ProgramBuilder.h
	src/ProgramModel/ProgramBuilder.cpp
	src/ProgramModel/ProgramModel.h
	src/ProgramModel/ProgramModel.cpp
	src/ProgramModel/ValidationFailure.h
	src/ProgramModel/ValidationFailure.cpp
	src/ProgramModel/ValidationUtils.h
	src/ProgramModel/ValidationUtils.cpp
	src/Interface_Assembler.h
	src/Interface_Assembler.cpp
	src/Interface_Exception.h
	src/Interface_Exception.cpp
	src/Interface_Version.cpp
)

add_library(${TARGETNAME_LIBV2MPASM} SHARED ${PUBLIC_HEADERS_ALL} ${SOURCES_ALL})

target_include_directories(${TARGETNAME_LIBV2MPASM} PUBLIC include)
target_include_directories(${TARGETNAME_LIBV2MPASM} PRIVATE src)

target_link_libraries(${TARGETNAME_LIBV2MPASM} PRIVATE
	${TARGETNAME_LIBBASEUTIL}
	${TARGETNAME_LIBSHAREDCOMPONENTS}
	cwalk
)

target_compile_definitions(${TARGETNAME_LIBV2MPASM} PRIVATE "V2MPASM_PRODUCER")

install(TARGETS ${TARGETNAME_LIBV2MPASM})
install(FILES ${PUBLIC_HEADERS_ALL} DESTINATION include/${TARGETNAME_LIBV2MPASM})
