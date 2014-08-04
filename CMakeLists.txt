##
# StatsD Client
#
# Copyright (c) 2012-2014 Axel Etcheverry
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with this source code.
##
cmake_minimum_required(VERSION 2.8.7)

project(StatsD CXX)

# Set some nicer output dirs.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# get git tag
execute_process(COMMAND git describe --always --tag OUTPUT_VARIABLE tag_version OUTPUT_STRIP_TRAILING_WHITESPACE)
string(SUBSTRING ${tag_version} 1 -1 tag_version)
set(STATSD_VERSION ${tag_version})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp @ONLY)

# enable valgrind
set(CMAKE_MEMORYCHECK_COMMAND valgrind)
set(CMAKE_MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1 -v --track-origins=yes --leak-check=full --show-reachable=yes --trace-children=yes --child-silent-after-fork=yes -q")

find_file(HAVE_VALGRIND "valgrind")

add_definitions(
    -std=c++11
    -Wall
    -pedantic
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(statsd SHARED src/statsd.cpp include/statsd.hpp include/version.hpp)
add_library(statsd_static STATIC src/statsd.cpp include/statsd.hpp include/version.hpp)

add_executable(
    statsd-test
    test/statsd.cpp
)

target_link_libraries(statsd-test statsd pthread)

enable_testing()

function(add_memcheck_test name binary)
    set(memcheck_command "${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS}")
    separate_arguments(memcheck_command)
    add_test(${name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${binary})
    if(HAVE_VALGRIND)
        add_test(memcheck_${name} ${memcheck_command} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${binary})
    endif()
endfunction(add_memcheck_test)

add_memcheck_test(StatsD statsd-test)

install(FILES include/statsd.hpp DESTINATION include)

install(TARGETS statsd_static statsd
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
