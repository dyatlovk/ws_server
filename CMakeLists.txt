cmake_minimum_required(VERSION 3.26)
project(server++)

option(SRV_BUILD_TESTS "Build unit tests" OFF)

# vendors
set(VENDORS ${PROJECT_SOURCE_DIR}/vendor)

# fmt lib
set(FMT_INCLUDE ${VENDORS}/fmt/include)
set(FMTLIB "${VENDORS}/fmt/linux/libfmt.a")

# Reflex lib
set(REFLEX_INCLUDE ${VENDORS}/reflex/include)
set(REFLEX "${VENDORS}/reflex/libreflex_static_lib.a")

# MiniJson lib
set(MINJSON_INCLUDE ${VENDORS}/minijson/include)
set(MINJSON "${VENDORS}/minijson/libJson.a")

add_subdirectory(libs/args)
add_subdirectory(src)

# unit test
if(SRV_BUILD_TESTS)
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(libs/args/tests)
endif()

if(SRV_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

#examples
option(SRV_BUILD_EXAMPLES "Build examples" OFF)
if(SRV_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
