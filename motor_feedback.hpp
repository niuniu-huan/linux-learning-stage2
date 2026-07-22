#pragma once

#include <array>
#include <cstdint>

struct MotorFeedback {
    std::uint16_t encoder_position;
    std::int16_t speed_rpm;
    std::int16_t current_ma;
    std::uint8_t temperature_c;
    bool enabled;
    bool fault;
};

MotorFeedback decode_motor_feedback(
    std::uint32_t can_id,
    const std::array<std::uint8_t, 8>& data);