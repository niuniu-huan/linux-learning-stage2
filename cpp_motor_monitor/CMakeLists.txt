cmake_minimum_required(VERSION 3.22)

project(cpp_motor_monitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(motor_driver
    src/motor.cpp
)

target_include_directories(motor_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(motor_driver PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(cpp_motor_monitor
    src/main.cpp
)

target_link_libraries(cpp_motor_monitor PRIVATE motor_driver)

enable_testing()

add_executable(motor_test
    tests/motor_test.cpp
)

target_link_libraries(motor_test PRIVATE motor_driver)

add_test(NAME motor_test COMMAND motor_test)