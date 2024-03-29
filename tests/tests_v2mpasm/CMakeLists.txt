add_executable(V2MPAsm_Tests
	src/AddTests.cpp
	src/AsgnTests.cpp
	src/BitwTests.cpp
	src/CbxTests.cpp
	src/DivTests.cpp
	src/ExceptionAPI.cpp
	src/ExceptionIDs.h
	src/Labels.cpp
	src/LanguageFeatures.cpp
	src/LdstTests.cpp
	src/Main.cpp
	src/MulTests.cpp
	src/NopTests.cpp
	src/ProgramVerification.h
	src/ProgramVerification.cpp
	src/ShftTests.cpp
	src/SigTests.cpp
	src/SingleInstructions.cpp
	src/StkTests.cpp
	src/SubTests.cpp
)

target_link_libraries(V2MPAsm_Tests PRIVATE
	Catch2::Catch2
	${TARGETNAME_LIBV2MPASM}
	${TARGETNAME_TESTUTIL}
	${TARGETNAME_LIBV2MP}
)

target_include_directories(V2MPAsm_Tests PRIVATE src)

if(WIN32)
	add_custom_command(TARGET V2MPAsm_Tests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:${TARGETNAME_LIBV2MPASM}>
		$<TARGET_FILE_DIR:V2MPAsm_Tests>
	)
endif()

include(CTest)
include(Catch)
catch_discover_tests(V2MPAsm_Tests)
