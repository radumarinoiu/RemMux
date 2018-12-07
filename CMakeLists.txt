cmake_minimum_required(VERSION 3.12)
project(RemMux C)

set(CMAKE_C_STANDARD 11)

add_executable(RemMux main.c Sources/constants.h Sources/client.c Sources/client.h Sources/server.c Sources/server.h Sources/console.c Sources/console.h)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(RemMux ${CURSES_LIBRARIES})