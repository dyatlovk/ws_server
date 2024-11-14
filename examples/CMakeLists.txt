cmake_minimum_required(VERSION 3.26)
project(examples)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(http_server)
add_subdirectory(mvc)
add_subdirectory(basic)
