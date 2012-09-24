cmake_minimum_required(VERSION 2.6)
set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIRS})

add_library(logpool SHARED logpool.c)
target_link_libraries(logpool kjson molder)

enable_testing()

set(TestLibs logpool)
macro(add_clib_test test_name)
	add_executable(${test_name} ${ARGN})
	target_link_libraries(${test_name} ${TestLibs})
	add_test(${test_name} ${test_name})
endmacro(add_clib_test test_name)

add_clib_test(test_logpool_open  test/test_logpool.c)
