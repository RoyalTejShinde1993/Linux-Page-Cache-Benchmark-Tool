cmake_minimum_required(VERSION 3.10)
project(pagebench)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(pagebench
    src/main.cpp
    src/benchmark.cpp
    src/buffered_io.cpp
    src/direct_io.cpp
    src/mmap_io.cpp
    src/metrics.cpp
)
