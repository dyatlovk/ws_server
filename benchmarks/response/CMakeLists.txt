cmake_minimum_required(VERSION 3.26)
project(benchmark_response)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(TEST_INCLUDE INTERNAL .)

add_executable(${PROJECT_NAME} ${SRC} main.c++)
target_include_directories(${PROJECT_NAME} PUBLIC ${FMT_INCLUDE} ${SRV_INCLUDE} ${TEST_INCLUDE} ${REFLEX_INCLUDE})
target_link_libraries(${PROJECT_NAME} ${FMTLIB} server:core ${REFLEX})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
