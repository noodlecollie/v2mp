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
	add_compile_options(
		-std=gnu11
	)
endif()

add_subdirectory(raylib-4.5.0 EXCLUDE_FROM_ALL)
