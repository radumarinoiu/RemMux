cmake_minimum_required(VERSION 3.13)
project(RemMux)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -fstack-protector-all -fsanitize=address -g -O0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")

add_executable(RemMux_Client client/main.cpp client/Client.h client/Client.cpp client/Child.h client/Child.cpp common/utils.cpp common/utils.h)
add_executable(RemMux_Server server/main.cpp server/Server.h server/Server.cpp server/Child.h server/Child.cpp server/Shell.h server/Shell.cpp)
add_executable(RemMux_Shell server/ShellTest.cpp server/Shell.h server/Shell.cpp)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(RemMux_Client ${CURSES_LIBRARIES})
target_link_libraries(RemMux_Shell ${CURSES_LIBRARIES})