cmake_minimum_required(VERSION 2.8)
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

enable_testing()
add_subdirectory(mold)
