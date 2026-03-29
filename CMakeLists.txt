cmake_minimum_required(VERSION 3.28)
project(DSA_Project_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(DSA_Project_2
        src/main.cpp
        test/test.cpp
        src/BPlusTree.cpp
        src/BPlusTree.hpp
        src/DataStructure.hpp
        src/SkipList.hpp
        src/SkipList.cpp
)

target_compile_options(DSA_Project_2 PRIVATE -g -Wall -Wextra -Wpedantic)
