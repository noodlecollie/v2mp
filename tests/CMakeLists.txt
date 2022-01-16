if(BUILD_TESTS AND BUILD_SHARED_LIBS)
	message(FATAL_ERROR "Linking tests against a shared library is not currently supported. Set BUILD_SHARED_LIBS to FALSE.")
endif()

# Always add Catch2, so that a `git clone --recursive` will download it correctly.
add_subdirectory(catch2)

# Only actually include the test projects if we are building tests.
if(BUILD_TESTS)
	add_subdirectory(tests)
endif()
