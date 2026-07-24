#include "motor_command_watchdog.hpp"

#include <chrono>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>

using namespace std::chrono_literals;

void expect_equal(const MotorCurrentCommand& actual,
                  const MotorCurrentCommand& expected,
                  const char* test_name) {
    if (actual.target_current_raw != expected.target_current_raw) {
        throw std::runtime_error(test_name);
    }
}

void test_watchdog_timeout_and_safety() {
    const MotorCurrentCommand requested{
        .target_current_raw = {1000, -1000, 500, 0},
    };
    const MotorCurrentCommand zero_command{};

    const MotorCommandSafety normal_safety{
        .output_enabled = true,
        .fault_active = false,
    };
    const MotorCommandSafety fault_safety{
        .output_enabled = true,
        .fault_active = true,
    };

    MotorCommandWatchdog watchdog{50ms};
    const auto start = MotorCommandWatchdog::Clock::time_point{};

    expect_equal(
        watchdog.command_for_send(normal_safety, start),
        zero_command,
        "watchdog should output zero before the first command");

    watchdog.update(requested, start);

    expect_equal(
        watchdog.command_for_send(normal_safety, start + 49ms),
        requested,
        "watchdog should keep a command before timeout");

    expect_equal(
        watchdog.command_for_send(fault_safety, start + 10ms),
        zero_command,
        "fault should override a valid watchdog command");

    expect_equal(
        watchdog.command_for_send(normal_safety, start + 50ms),
        zero_command,
        "watchdog should output zero at the timeout boundary");
}

int main() {
    try {
        test_watchdog_timeout_and_safety();
        std::cout << "All watchdog tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }
}