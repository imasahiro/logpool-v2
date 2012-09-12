include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckCSourceCompiles)

cmake_minimum_required(VERSION 2.8)
project(logpool)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

SET(CMAKE_C_FLAGS         "-Wall -Wno-variadic-macros")
SET(CMAKE_CXX_FLAGS       "-Wall -Wno-variadic-macros")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -g")
SET(CMAKE_C_FLAGS_DEBUG   "-O0 -g3")
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -g")
SET(CMAKE_CXX_FLAGS_DEBUG   "-Wall -O0 -g3")

set(MOLDER mold.c string_builder.c)
set(MOLDER_PLUGIN
		plugins/file.c
		plugins/filter.c
		plugins/multiplexer.c
		plugins/syslog.c
)

check_type_size("void *" SIZEOF_VOIDP)
check_type_size(long     SIZEOF_LONG)
check_type_size(int      SIZEOF_INT)
check_type_size(float    SIZEOF_FLOAT)
check_type_size(double   SIZEOF_DOUBLE)

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-fPIC)

add_library(molder ${MOLDER} ${MOLDER_PLUGIN})

########################################################
## Test cases
enable_testing()
set(TestLibs molder)
macro(add_clib_test test_name)
	add_executable(${test_name} ${ARGN})
	target_link_libraries(${test_name} ${TestLibs})
	add_test(${test_name} ${test_name})
endmacro(add_clib_test test_name)

add_clib_test(test_string_builder   test/test_string_builder.cpp)
add_clib_test(test_file     test/test_file.c)
#add_clib_test(test_syslog   test/test_syslog.c)
#add_clib_test(test_largefmt test/large_fmtdata.c)
#add_clib_test(test_filter   test/test_filter.c)
add_clib_test(test_multi    test/test_multiplex.c)
#add_clib_test(test_thread  test/test_thread.c)
#add_clib_test(test_trace   test/trace.c)
#add_clib_test(test_trace2  test/trace2.c)
