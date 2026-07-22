#include "motor_feedback.hpp"

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

int main() {
    const std::array<std::uint8_t, 8> frame_data{
        0x05, 0xDC, 0xFF, 0x88, 0x09, 0xC4, 0x3C, 0x01
    };

    const MotorFeedback feedback = decode_motor_feedback(0x201, frame_data);

    std::cout << "encoder_position: " << feedback.encoder_position << '\n'
              << "speed_rpm: " << feedback.speed_rpm << '\n'
              << "current_ma: " << feedback.current_ma << '\n'
              << "temperature_c: "
              << static_cast<int>(feedback.temperature_c) << '\n'
              << std::boolalpha
              << "enabled: " << feedback.enabled << '\n'
              << "fault: " << feedback.fault << '\n';
}