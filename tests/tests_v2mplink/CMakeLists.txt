add_executable(V2MPLink_Tests
	src/Main.cpp
)

target_link_libraries(V2MPLink_Tests PRIVATE
	Catch2::Catch2
	${TARGETNAME_LIBV2MPLINK}
	${TARGETNAME_TESTUTIL}
)

target_include_directories(V2MPLink_Tests PRIVATE src)

if(WIN32)
	add_custom_command(TARGET V2MPLink_Tests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:${TARGETNAME_LIBV2MPLINK}>
		$<TARGET_FILE_DIR:V2MPLink_Tests>
	)
endif()

include(CTest)
include(Catch)
catch_discover_tests(V2MPLink_Tests)
