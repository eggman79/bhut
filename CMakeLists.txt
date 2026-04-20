cmake_minimum_required(VERSION 3.15)
project(myapp LANGUAGES CXX ASM_NASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")

# Compile C++ with PIE
add_executable(myapp
    main.cpp
    ext.asm
)
