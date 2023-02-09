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
	src/Files/OutputFile.h
	src/Files/OutputFile.cpp
	src/Utils/StringUtils.cpp
	src/Parser/Parser.h
	src/Parser/Parser.cpp
	src/Parser/Tokeniser.h
	src/Parser/Tokeniser.cpp
	src/ProgramModel/Validators/AsgnCodeWordValidator.h
	src/ProgramModel/Validators/AsgnCodeWordValidator.cpp
	src/ProgramModel/Validators/BaseCodeWordValidator.h
	src/ProgramModel/Validators/BaseCodeWordValidator.cpp
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
	src/ProgramModel/Validators/ZeroArgCodeWordValidator.h
	src/ProgramModel/Validators/ZeroArgCodeWordValidator.cpp
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
)

target_include_directories(${TARGETNAME_V2MP_ASM} PUBLIC include)
target_include_directories(${TARGETNAME_V2MP_ASM} PRIVATE src)

target_link_libraries(${TARGETNAME_V2MP_ASM} PRIVATE
	${TARGETNAME_BASEUTIL}
	${TARGETNAME_SHARED_COMPONENTS}
	cwalk
)

target_compile_definitions(${TARGETNAME_V2MP_ASM} PRIVATE "V2MPASM_PRODUCER")
