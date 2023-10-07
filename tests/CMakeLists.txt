cmake_minimum_required(VERSION 3.26)
project(wsTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# check if socat is exist
find_program(SOCAT_EXIST "socat" REQUIRED)
if(!SOCAT_EXIST)
	message(FATAL_ERROR "this tests needed a socat")
endif()

file(GLOB SOURCES 
	main.c++ 
	${WS_INCLUDE}/io/sockets.c++
	${WS_INCLUDE}/io/sockets/inet_socket.c++
	${WS_INCLUDE}/io/sockets/local_socket.c++
	)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${FMT_INCLUDE} ${WS_INCLUDE})
target_link_libraries(${PROJECT_NAME} ${FMTLIB})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
