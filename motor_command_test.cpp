#include "motor_command.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>

void expect_equal(const MotorCurrentCommand& actual,
                  const MotorCurrentCommand& expected,
                  const char* test_name) {
    if (actual.target_current_raw != expected.target_current_raw) {
        throw std::runtime_error(test_name);
    }
}

void test_clamps_and_encodes_current() {
    const MotorCurrentCommand requested{
        .target_current_raw = {7000, -6000, 500, 0},
    };
    const MotorCommandSafety safety{
        .output_enabled = true,
        .fault_active = false,
    };

    const MotorCurrentCommand safe = make_safe_command(requested, safety);

    expect_equal(
        safe,
        MotorCurrentCommand{
            .target_current_raw = {5000, -5000, 500, 0},
        },
        "current command was not clamped correctly");

    const std::array<std::uint8_t, 8> expected_data{
        0x13, 0x88,  // 5000
        0xEC, 0x78,  // -5000
        0x01, 0xF4,  // 500
        0x00, 0x00,  // 0
    };

    if (encode_motor_currents(safe) != expected_data) {
        throw std::runtime_error("safe command was encoded incorrectly");
    }
}

void test_disables_output_on_fault_or_disable() {
    const MotorCurrentCommand requested{
        .target_current_raw = {1000, -1000, 2000, -2000},
    };
    const MotorCurrentCommand zero_command{};

    expect_equal(
        make_safe_command(requested, {.output_enabled = false,
                                      .fault_active = false}),
        zero_command,
        "disabled output was not zero");

    expect_equal(
        make_safe_command(requested, {.output_enabled = true,
                                      .fault_active = true}),
        zero_command,
        "fault output was not zero");
}

int main() {
    try {
        test_clamps_and_encodes_current();
        test_disables_output_on_fault_or_disable();
        std::cout << "All motor command safety tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }
}