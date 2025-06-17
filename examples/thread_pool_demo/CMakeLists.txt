cmake_minimum_required(VERSION 3.26)

project(thread_pool_demo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the executable
add_executable(thread_pool_demo main.c++)

# Link libraries (they're already built by the main project)
target_link_libraries(thread_pool_demo
    PRIVATE
    core
    argsparser
    ${CMAKE_SOURCE_DIR}/vendor/fmt/linux/libfmt.a
    ${CMAKE_SOURCE_DIR}/vendor/minijson/libJson.a
    ${CMAKE_SOURCE_DIR}/vendor/reflex/libreflex_static_lib.a
)

# Include directories
target_include_directories(thread_pool_demo
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/fmt/include
    ${CMAKE_SOURCE_DIR}/vendor/minijson/include
    ${CMAKE_SOURCE_DIR}/vendor/reflex/include
)
