cmake_minimum_required(VERSION 3.0)
project(Math_Parser)
project(Math_Parser_Test)

set(CMAKE_CXX_STANDARD 14)

add_library(Math_Parser math_parse.cpp example.cpp)

add_executable(Math_Parser_Test example.cpp math_parse.cpp)