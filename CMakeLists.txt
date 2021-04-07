cmake_minimum_required(VERSION 3.11)
project(pico_pytagoras VERSION 0.1)

option(BUILD_TESTS "Whether to build the test library at all" OFF)

set(PYTAGORAS_LIBNAME pico_pytagoras)

add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()