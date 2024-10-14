cmake_minimum_required(VERSION 3.26)
project(argsparser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(ARGS_INCLUDES public)
file(GLOB SRC src/parser.c++)

add_library(${PROJECT_NAME} STATIC ${SRC})
add_library(server:args ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${FMT_INCLUDE} ${ARGS_INCLUDES})
target_link_libraries(${PROJECT_NAME} ${FMTLIB})

# if(SRV_BUILD_TESTS)
#     add_subdirectory(tests)
# endif()
