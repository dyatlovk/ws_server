cmake_minimum_required(VERSION 3.26)
project(websocket)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
string(TIMESTAMP COMPILE_TIME "%Y-%m-%d %H:%M:%S")

# Uses Git to find the current git branch and commit.
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)

execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)

# vendors
set(VENDORS ${PROJECT_SOURCE_DIR}/vendor)

# fmt lib
set(FMT_INCLUDE ${VENDORS}/fmt/include)
set(FMTLIB "${VENDORS}/fmt/linux/libfmt.a")

# main app
set(WS_INCLUDE "${PROJECT_SOURCE_DIR}/src")
file(GLOB WS_SOURCES 
    src/main.c++
    src/server/simple_server.c++
    src/io/epoll/epoll.c++
    src/io/sockets/inet_socket.c++
    src/io/sockets/local_socket.c++
    )

# Build type
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build debug")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Wall -Wextra -ggdb3")
endif()

# unit test
option(WS_BUILD_TESTS "Build unit tests" OFF)
if (WS_BUILD_TESTS)
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    enable_testing()
    add_subdirectory(tests)
endif ()

add_executable(${PROJECT_NAME} ${WS_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${WS_INCLUDE} ${FMT_INCLUDE})
target_link_libraries(${PROJECT_NAME} ${FMTLIB})
