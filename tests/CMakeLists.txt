# Always add Catch2, so that a `git clone --recursive` will download it correctly.
add_subdirectory(catch2)

if(BUILD_TESTING AND NOT CATCH_BUILD_TESTING)
	message(FATAL_ERROR "BUILD_TESTING was set, but CATCH_BUILD_TESTING was not. Both variables must be set to build tests.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/catch2/Catch2/contrib")

# Only actually include the test projects if we are building tests.
if(BUILD_TESTING)
	set(TARGETNAME_TESTUTIL TestUtil)

	add_subdirectory(testutil)
	add_subdirectory(tests_v2mp)
	add_subdirectory(tests_v2mpasm)
	add_subdirectory(tests_v2mplink)
endif()
