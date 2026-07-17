cmake_minimum_required(VERSION 3.22)

project(cpp_motor_monitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpp_motor_monitor
    src/main.cpp
    src/motor.cpp
)

target_include_directories(cpp_motor_monitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(cpp_motor_monitor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)