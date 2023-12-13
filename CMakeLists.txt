cmake_minimum_required(VERSION 3.10)
project(lab06)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

include_directories("include")
add_executable(lab06 "src/main.c")
add_library(lab06_lib STATIC "src/lib.c")
target_link_libraries(lab06 lab06_lib)
