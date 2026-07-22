#include "motor_command.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>

int main() {
    const MotorCurrentCommand command{
        .target_current_raw = {1000, -500, 0, 2500},
    };

    const auto data = encode_motor_currents(command);

    std::cout << "CAN ID: 0x200\nData:";
    for (const std::uint8_t byte : data) {
        std::cout << " 0x"
                  << std::uppercase << std::hex
                  << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(byte);
    }
    std::cout << '\n';
}