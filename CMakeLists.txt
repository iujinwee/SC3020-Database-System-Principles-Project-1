cmake_minimum_required(VERSION 3.20)
project(SC3020_Database_System_Principles)

set(CMAKE_CXX_STANDARD 17)

add_executable(SC3020_Database_System_Principles src/main.cpp
        src/storage/MemoryPool.cpp
        src/storage/MemoryPool.h
        src/storage/Record.cpp
        src/storage/Record.h
        src/indexing/BPlusTree.cpp
        src/indexing/BPlusTree.h
        src/storage/Block.cpp
        src/storage/Block.h
)
