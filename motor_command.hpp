#pragma once

#include <array>
#include <cstdint>

struct MotorCurrentCommand {
    std::array<std::int16_t, 4> target_current_raw;
};

struct MotorCommandSafety {
    bool output_enabled;
    bool fault_active;
};

constexpr std::int16_t kMaxMotorCurrentRaw = 5000;

std::array<std::uint8_t, 8> encode_motor_currents(
    const MotorCurrentCommand& command);

MotorCurrentCommand make_safe_command(
    const MotorCurrentCommand& requested,
    const MotorCommandSafety& safety);