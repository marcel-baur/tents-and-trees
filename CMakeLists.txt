cmake_minimum_required(VERSION 3.15)
project(SAT)

set(CMAKE_CXX_STANDARD 20)

add_executable(SAT main.cpp Field.h Field.cpp Cell.cpp Cell.h)