cmake_minimum_required(VERSION 3.26)
project(wsTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB SOURCES main.c++)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${FMT_INCLUDE} ${WS_INCLUDE})
target_link_libraries(${PROJECT_NAME} ${FMTLIB})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
