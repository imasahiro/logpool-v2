include(CheckIncludeFiles)
include(CheckLibraryExists)

cmake_minimum_required(VERSION 2.8)
project(stream)
set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

find_library(HAVE_LIBDL    NAMES dl)
find_library(HAVE_PTHREAD  NAMES pthread)
find_library(HAVE_LIBEVENT NAMES event)
find_library(HAVE_LIBEVENT_PTHREADS NAMES event_pthreads)

check_include_files(pthread.h      HAVE_PTHREAD_H)
check_include_files(event2/event.h HAVE_LIBEVENT_H)

if(NOT HAVE_PTHREAD_H)
	message(FATAL_ERROR "pthread.h not found: ${HAVE_PTHREAD_H}")
endif(NOT HAVE_PTHREAD_H)

if(NOT HAVE_LIBEVENT_H)
	message(FATAL_ERROR "event2/event.h not found")
endif(NOT HAVE_LIBEVENT_H)

if(NOT HAVE_PTHREAD)
	message(FATAL_ERROR "libpthread not found")
endif(NOT HAVE_PTHREAD)

if(NOT HAVE_LIBEVENT)
	message(FATAL_ERROR "libevent not found")
endif(NOT HAVE_LIBEVENT)

add_definitions(-fPIC)

add_library(stream io.c stream.c)
target_link_libraries(stream ${HAVE_PTHREAD} ${HAVE_LIBEVENT} ${HAVE_LIBEVENT_PTHREADS} ${HAVE_LIBDL})

add_clib_test(test_thread  test/test_thread.c)
