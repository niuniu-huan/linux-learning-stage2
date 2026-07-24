cmake_minimum_required(VERSION 3.20)

project(motor_command_safety LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(motor_command STATIC motor_command.cpp)
target_include_directories(motor_command PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(motor_command_watchdog STATIC motor_command_watchdog.cpp)
target_include_directories(
    motor_command_watchdog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(motor_command_watchdog PUBLIC motor_command)

add_executable(motor_command_test motor_command_test.cpp)
target_link_libraries(motor_command_test PRIVATE motor_command)

add_executable(motor_command_watchdog_test motor_command_watchdog_test.cpp)
target_link_libraries(
    motor_command_watchdog_test PRIVATE motor_command_watchdog)

enable_testing()

add_test(NAME motor_command_safety_test COMMAND motor_command_test)
add_test(NAME motor_command_watchdog_test COMMAND motor_command_watchdog_test)