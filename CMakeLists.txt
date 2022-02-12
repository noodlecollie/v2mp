cmake_minimum_required(VERSION 3.12)

include (TestBigEndian)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

TEST_BIG_ENDIAN(HOST_IS_BIG_ENDIAN)

if(HOST_IS_BIG_ENDIAN)
	message(FATAL "Compilation is not currently supported for big-endian systems.")
endif()

set(TARGETNAME_V2MP V2MP)
set(TARGETNAME_V2MP_INTERNAL V2MPInternal)
set(TARGETNAME_V2MP_ASM V2MPAsm)
set(TARGETNAME_ASSEMBLER Assembler)
set(TARGETNAME_BASEUTIL BaseUtil)

option(BUILD_SHARED_LIBS "If set, builds shared libraries; otherwise, builds static libraries." YES)
option(BUILD_TESTING "If set, builds tests and links with Catch2" NO)

if(BUILD_TESTING)
	enable_testing()
endif()

# Used for generated headers:
if(BUILD_SHARED_LIBS)
	set(V2MP_IS_SHARED_BUILD YES)
else()
	set(V2MP_IS_SHARED_BUILD NO)
endif()

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

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(thirdparty)
