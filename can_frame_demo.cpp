#include <cstdint>
#include <iomanip>
#include <iostream>

#include <linux/can.h>

int main() {
    constexpr canid_t motor_command_id = 0x201;
    constexpr std::int16_t target_current_ma = 1500;

    can_frame frame{};
    frame.can_id = motor_command_id;
    frame.len = 8;

    const std::uint16_t encoded_current =
        static_cast<std::uint16_t>(target_current_ma);

    // 示例协议：前两个字节为大端序的目标电流，单位 mA。
    frame.data[0] = static_cast<std::uint8_t>(encoded_current >> 8);
    frame.data[1] = static_cast<std::uint8_t>(encoded_current & 0xFF);

    std::cout << "CAN ID: 0x" << std::hex << frame.can_id << std::dec << '\n';
    std::cout << "DLC: " << static_cast<int>(frame.len) << '\n';
    std::cout << "Data: ";

    for (int index = 0; index < frame.len; ++index) {
        std::cout << "0x"
                  << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(frame.data[index]) << ' ';
    }
    std::cout << std::dec << '\n';

    const std::uint16_t received_raw =
        (static_cast<std::uint16_t>(frame.data[0]) << 8) |
        static_cast<std::uint16_t>(frame.data[1]);

    const auto received_current_ma =
        static_cast<std::int16_t>(received_raw);

    std::cout << "Decoded current: "
              << received_current_ma << " mA\n";

    return 0;
}