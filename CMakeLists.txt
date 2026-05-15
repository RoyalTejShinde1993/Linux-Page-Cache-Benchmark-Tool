cmake_minimum_required(VERSION 3.10)
project(pagebench LANGUAGES CXX)

option(PAGEBENCH_USE_CXX23 "Compile with C++23 when supported" ON)

if(PAGEBENCH_USE_CXX23)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BENCH_SOURCES
    src/main.cpp
    src/benchmark.cpp
    src/metrics.cpp
    src/buffered_io.cpp
)

if(UNIX)
    list(APPEND BENCH_SOURCES
        src/direct_io.cpp
        src/mmap_io.cpp
    )
else()
    message(WARNING "Direct IO and mmap benchmarks are supported only on Unix-like systems.")
endif()

add_executable(pagebench ${BENCH_SOURCES})
target_include_directories(pagebench PRIVATE include)

if(MSVC)
    target_compile_options(pagebench PRIVATE /W4 /permissive-)
else()
    target_compile_options(pagebench PRIVATE -Wall -Wextra -Wpedantic)
endif()
