cmake_minimum_required(VERSION 3.26)
project(core)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# main app
set(SRV_INCLUDE "${PROJECT_SOURCE_DIR}")
file(GLOB SRV_SOURCES
    io/epoll/epoll.c++
    io/sockets/inet_socket.c++
    io/sockets/local_socket.c++
    http/parser.c++
    http/uri.c++
    http/stream.c++
    http/response.c++
    http/router.c++
    utils/thread_pool.c++
)

# Build type
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build debug")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Wall -Wextra -ggdb3")
endif()

add_library(${PROJECT_NAME} STATIC ${SRV_SOURCES})
add_library(server:core ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${SRV_INCLUDE} ${FMT_INCLUDE})
target_link_libraries(${PROJECT_NAME} ${FMTLIB})
