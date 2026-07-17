cmake_minimum_required(VERSION 3.22)

project(motor_monitor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(motor_monitor
    src/main.c
    src/motor.c
)

target_include_directories(motor_monitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(motor_monitor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)