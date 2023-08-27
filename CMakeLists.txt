cmake_minimum_required(VERSION 3.26)
project(SC3020_Database_System_Principles)

set(CMAKE_CXX_STANDARD 17)

add_executable(SC3020_Database_System_Principles src/main.cpp
        src/storage/types.h
        src/storage/memory_pool.cpp
        src/storage/memory_pool.h
)
