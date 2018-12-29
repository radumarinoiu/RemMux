cmake_minimum_required(VERSION 3.13)
project(RemMux)

set(CMAKE_CXX_STANDARD 17)

add_executable(RemMux_Client src/client.cpp src/client.h src/constants.h)
add_executable(RemMux_Server src/server.cpp src/server.h src/constants.h)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(RemMux_Client ${CURSES_LIBRARIES})