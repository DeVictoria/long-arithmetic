cmake_minimum_required(VERSION 3.24)
project(long-arithmetic)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(long-arithmetic
        LN.cpp
        LN.h
        main.cpp
        return_codes.h)
