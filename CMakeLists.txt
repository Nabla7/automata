cmake_minimum_required(VERSION 3.24)
project(code)

set(CMAKE_CXX_STANDARD 17)

add_executable(code main.cpp DFA.h DFA.cpp DFA.cpp)
