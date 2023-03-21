cmake_minimum_required(VERSION 3.12)

include(TestBigEndian)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)

	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

cmake_policy(SET CMP0092 NEW)

project(V2MP)

find_package(Git QUIET REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

TEST_BIG_ENDIAN(HOST_IS_BIG_ENDIAN)

if(HOST_IS_BIG_ENDIAN)
	message(FATAL "Compilation is not currently supported for big-endian systems.")
endif()

set(TARGETNAME_V2MPASM V2MPAsm)
set(TARGETNAME_V2MPLINK V2MPLink)
set(TARGETNAME_LIBV2MP LibV2MP)
set(TARGETNAME_LIBV2MPINTERNAL LibV2MPInternal)
set(TARGETNAME_LIBV2MPASM LibV2MPAsm)
set(TARGETNAME_LIBV2MPLINK LibV2MPLink)
set(TARGETNAME_LIBBASEUTIL LibBaseUtil)
set(TARGETNAME_LIBSHAREDCOMPONENTS LibSharedComponents)
set(TARGETNAME_LIBTOOLCHAINCOMPONENTS LibToolchainComponents)
set(TARGETNAME_UTHASH uthash)
set(TARGETNAME_ARGPARSE argparse)
set(TARGETNAME_CWALK cwalk)

include(target_versions)

option(BUILD_TESTING "If set, builds tests and links with Catch2" NO)

if(BUILD_TESTING)
	enable_testing()
endif()

# Store Git hash in VCS_COMMIT_ID
execute_process(
	COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	OUTPUT_VARIABLE VCS_COMMIT_ID
	ERROR_QUIET
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Make configure step depend on git index
set_property(GLOBAL APPEND
	PROPERTY CMAKE_CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/.git/index"
)

# Turn up warning levels as high as is feasible, and treat warnings as errors.
# Correct code should be warning-free at all times, no exceptions.
if(MSVC)
	add_compile_options(/W4 /WX)
	add_link_options(/WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
	add_link_options(-Wl,--fatal-warnings)
endif()

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(thirdparty)
