set(WITH_PIC ON)
set(BUILD_SHARED_LIBS OFF)

if(MSVC)
	add_compile_options(
		/wd4127
		/wd4100
		/wd4244
		/wd4245
		/wd4456
		/wd4457
		/wd4018
		/wd4267
		/wd4389
		/wd4706
		/wd4090
		/wd4152
		/wd4701
		/wd4057
		/wd4702
		/wd4005
	)
else()
	# This enables GNU extensions so that readlink is accessible.
	# -std=gnu11 seems to work for Clang but not, weirdly, for GCC >:c
	add_compile_definitions(_POSIX_C_SOURCE=200809L)

	add_compile_options(
		-Wno-discarded-qualifiers
		-Wno-unused-result
		-Wno-stringop-overflow
	)
endif()

add_subdirectory(raylib-4.5.0 EXCLUDE_FROM_ALL)
