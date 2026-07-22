#pragma once

#include <array>
#include <cstdint>

struct MotorCurrentCommand {
    std::array<std::int16_t, 4> target_current_raw;
};

std::array<std::uint8_t, 8> encode_motor_currents(
    const MotorCurrentCommand& command);