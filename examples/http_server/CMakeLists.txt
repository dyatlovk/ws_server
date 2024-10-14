cmake_minimum_required(VERSION 3.26)
project(http_server)

file(GLOB SRC main.c++ server.c++)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${SRV_INCLUDE} ${ARGS_INCLUDES} ${FMT_INCLUDE})
target_link_libraries(${PROJECT_NAME} ${FMTLIB} server:core server:args)
