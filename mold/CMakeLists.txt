include(CheckTypeSize)

cmake_minimum_required(VERSION 2.8)
project(mold)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIRS})

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

add_definitions(-fPIC)
add_library(molder ${MOLDER} ${MOLDER_PLUGIN})

########################################################
## Test cases
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
