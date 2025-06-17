cmake_minimum_required(VERSION 3.26)
project(logging_demo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(${PROJECT_NAME} main.c++)
target_include_directories(${PROJECT_NAME} PUBLIC ${SRV_INCLUDE} ${FMT_INCLUDE})
target_link_libraries(${PROJECT_NAME} server:core ${FMTLIB})
