cmake_minimum_required(VERSION 3.26)
project(labwork8_Aram_schoolboy)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)
