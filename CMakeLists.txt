cmake_minimum_required(VERSION 3.20)

project(motor_command_safety LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(motor_command STATIC motor_command.cpp)
target_include_directories(motor_command PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(motor_command_test motor_command_test.cpp)
target_link_libraries(motor_command_test PRIVATE motor_command)

enable_testing()
add_test(NAME motor_command_safety_test COMMAND motor_command_test)