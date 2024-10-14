cmake_minimum_required(VERSION 3.26)
project(argsparser_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(${PROJECT_NAME} ${SRC} main.c++)
target_link_libraries(${PROJECT_NAME} server:args)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
