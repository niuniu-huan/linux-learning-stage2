#include "motor_command.hpp"

#include <algorithm>
#include <cstddef>

namespace {

void write_i16_be(std::array<std::uint8_t, 8>& data,
                  std::size_t offset,
                  std::int16_t value) {
    const std::uint16_t raw = static_cast<std::uint16_t>(value);

    data.at(offset) = static_cast<std::uint8_t>(raw >> 8U);
    data.at(offset + 1) = static_cast<std::uint8_t>(raw & 0xFFU);
}

}  // namespace

std::array<std::uint8_t, 8> encode_motor_currents(
    const MotorCurrentCommand& command) {
    std::array<std::uint8_t, 8> data{};

    for (std::size_t motor = 0; motor < command.target_current_raw.size();
         ++motor) {
        write_i16_be(data, motor * 2, command.target_current_raw.at(motor));
    }

    return data;
}

MotorCurrentCommand make_safe_command(
    const MotorCurrentCommand& requested,
    const MotorCommandSafety& safety) {
    if (!safety.output_enabled || safety.fault_active) {
        return {};
    }

    MotorCurrentCommand safe_command{};

    for (std::size_t motor = 0;
         motor < requested.target_current_raw.size();
         ++motor) {
        safe_command.target_current_raw.at(motor) = std::clamp(
            requested.target_current_raw.at(motor),
            static_cast<std::int16_t>(-kMaxMotorCurrentRaw),
            kMaxMotorCurrentRaw);
    }

    return safe_command;
}