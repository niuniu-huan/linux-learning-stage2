#include "motor_command_watchdog.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>

using namespace std::chrono_literals;

void print_command(const char* label, const MotorCurrentCommand& command) {
    std::cout << label << ":";

    for (const std::int16_t current : command.target_current_raw) {
        std::cout << ' ' << current;
    }

    std::cout << '\n';
}

int main() {
    const MotorCommandSafety normal_safety{
        .output_enabled = true,
        .fault_active = false,
    };
    const MotorCurrentCommand requested{
        .target_current_raw = {1000, -1000, 500, 0},
    };

    MotorCommandWatchdog watchdog{50ms};
    const auto start = MotorCommandWatchdog::Clock::now();

    print_command(
        "Before first command",
        watchdog.command_for_send(normal_safety, start));

    watchdog.update(requested, start);

    print_command(
        "49 ms after command",
        watchdog.command_for_send(normal_safety, start + 49ms));

    print_command(
        "50 ms after command",
        watchdog.command_for_send(normal_safety, start + 50ms));
}