cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)

	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(V2MP)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_SHARED_LIBS "If set, builds shared libraries; otherwise, builds static libraries." YES)
option(BUILD_TESTS "If set, test suite is built." NO)

# Store Git hash in VCS_COMMIT_ID
execute_process(
	COMMAND "${GIT_EXECUTABLE}" describe --always HEAD
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	RESULT_VARIABLE res
	OUTPUT_VARIABLE VCS_COMMIT_ID
	ERROR_QUIET
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Make configure step depend on git index
set_property(GLOBAL APPEND
	PROPERTY CMAKE_CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/.git/index"
)

add_compile_definitions("GIT_COMMIT_HASH=\"${VCS_COMMIT_ID}\"")

# Turn up warning levels as high as is feasible, and treat warnings as errors.
# Correct code should be warning-free at all times, no exceptions.
if(MSVC)
	add_compile_options(/W4 /WX)
	add_link_options(/WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
	add_link_options(-Wl,--fatal-warnings)
endif()

add_subdirectory(v2mp)

if(BUILD_TESTS)
	add_subdirectory(tests)
endif()
