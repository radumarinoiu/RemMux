cmake_minimum_required(VERSION 3.13)
project(RemMux)

set(CMAKE_CXX_STANDARD 17)

add_executable(RemMux src/constants.h src/client.cpp src/client.h src/server.cpp src/server.h)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(RemMux ${CURSES_LIBRARIES})