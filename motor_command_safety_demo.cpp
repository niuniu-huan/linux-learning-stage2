#include "motor_command.hpp"

#include <iostream>

void print_command(const char* label, const MotorCurrentCommand& command) {
    std::cout << label << ":";

    for (const std::int16_t current : command.target_current_raw) {
        std::cout << ' ' << current;
    }

    std::cout << '\n';
}

int main() {
    const MotorCurrentCommand requested{
        .target_current_raw = {7000, -6000, 500, 0},
    };

    const MotorCommandSafety enabled_no_fault{
        .output_enabled = true,
        .fault_active = false,
    };

    const MotorCommandSafety fault_active{
        .output_enabled = true,
        .fault_active = true,
    };

    print_command("Requested", requested);
    print_command("Enabled, no fault",
                  make_safe_command(requested, enabled_no_fault));
    print_command("Fault active",
                  make_safe_command(requested, fault_active));
}