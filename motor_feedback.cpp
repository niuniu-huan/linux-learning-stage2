#include "motor_feedback.hpp"

#include <stdexcept>

namespace {

std::uint16_t read_u16_be(const std::array<std::uint8_t, 8>& data,
                          std::size_t offset) {
    return (static_cast<std::uint16_t>(data.at(offset)) << 8) |
           static_cast<std::uint16_t>(data.at(offset + 1));
}

std::int16_t read_i16_be(const std::array<std::uint8_t, 8>& data,
                         std::size_t offset) {
    return static_cast<std::int16_t>(read_u16_be(data, offset));
}

}  // namespace

MotorFeedback decode_motor_feedback(
    std::uint32_t can_id,
    const std::array<std::uint8_t, 8>& data) {
    if (can_id != 0x201) {
        throw std::runtime_error("unexpected CAN ID");
    }

    const std::uint8_t status = data.at(7);

    return {
        .encoder_position = read_u16_be(data, 0),
        .speed_rpm = read_i16_be(data, 2),
        .current_ma = read_i16_be(data, 4),
        .temperature_c = data.at(6),
        .enabled = (status & 0x01U) != 0,
        .fault = (status & 0x02U) != 0,
    };
}