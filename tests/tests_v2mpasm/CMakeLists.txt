add_executable(V2MPAsm_Tests
	src/Dummy.cpp
	src/Main.cpp
	src/SingleInstructions.cpp
)

target_link_libraries(V2MPAsm_Tests PRIVATE
	Catch2::Catch2
	${TARGETNAME_V2MP_ASM}
	${TARGETNAME_TESTUTIL}
	${TARGETNAME_V2MP_INTERNAL}
)

target_include_directories(V2MPAsm_Tests PRIVATE src)

include(CTest)
include(Catch)
catch_discover_tests(V2MPAsm_Tests)
