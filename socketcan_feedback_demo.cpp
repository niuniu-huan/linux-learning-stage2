#include "motor_feedback.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

void check(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(
            std::string(message) + ": " + std::strerror(errno));
    }
}

int main() {
    try {
        const int socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        check(socket_fd >= 0, "socket");

        const int receive_own_messages = 1;
        check(setsockopt(socket_fd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS,
                    &receive_own_messages,
                    sizeof(receive_own_messages)) >= 0,
            "setsockopt CAN_RAW_RECV_OWN_MSGS");

        struct ifreq ifr {};
        std::strncpy(ifr.ifr_name, "vcan0", IFNAMSIZ - 1);
        check(ioctl(socket_fd, SIOCGIFINDEX, &ifr) >= 0, "ioctl SIOCGIFINDEX");

        const struct can_filter filter {
            .can_id = 0x201,
            .can_mask = CAN_SFF_MASK,
        };
        check(setsockopt(socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER,
                         &filter, sizeof(filter)) >= 0,
              "setsockopt CAN_RAW_FILTER");

        struct sockaddr_can address {};
        address.can_family = AF_CAN;
        address.can_ifindex = ifr.ifr_ifindex;
        check(bind(socket_fd, reinterpret_cast<struct sockaddr*>(&address),
                   sizeof(address)) >= 0,
              "bind");

        const std::array<std::uint8_t, 8> feedback_data{
            0x05, 0xDC,  // encoder_position = 1500
            0xFF, 0x88,  // speed_rpm = -120
            0x09, 0xC4,  // current_ma = 2500
            0x3C,        // temperature_c = 60
            0x01,        // enabled = true, fault = false
        };

        struct can_frame tx_frame {};
        tx_frame.can_id = 0x201;
        tx_frame.can_dlc = feedback_data.size();
        std::copy(feedback_data.begin(), feedback_data.end(), tx_frame.data);

        check(write(socket_fd, &tx_frame, sizeof(tx_frame)) ==
                  static_cast<ssize_t>(sizeof(tx_frame)),
              "write CAN frame");

        struct pollfd poll_fd {
            .fd = socket_fd,
            .events = POLLIN,
            .revents = 0,
        };
        const int poll_result = poll(&poll_fd, 1, 1000);
        if (poll_result == 0) {
            throw std::runtime_error("timeout waiting for CAN feedback");
        }
        check(poll_result > 0, "poll");

        struct can_frame rx_frame {};
        check(read(socket_fd, &rx_frame, sizeof(rx_frame)) ==
                  static_cast<ssize_t>(sizeof(rx_frame)),
              "read CAN frame");

        const std::array<std::uint8_t, 8> received_data{
            rx_frame.data[0], rx_frame.data[1],
            rx_frame.data[2], rx_frame.data[3],
            rx_frame.data[4], rx_frame.data[5],
            rx_frame.data[6], rx_frame.data[7],
        };

        const MotorFeedback feedback = decode_motor_feedback(
            rx_frame.can_id & CAN_SFF_MASK, received_data);

        std::cout << "CAN ID: 0x" << std::hex
                  << (rx_frame.can_id & CAN_SFF_MASK) << std::dec << '\n'
                  << "encoder_position: " << feedback.encoder_position << '\n'
                  << "speed_rpm: " << feedback.speed_rpm << '\n'
                  << "current_ma: " << feedback.current_ma << '\n'
                  << "temperature_c: "
                  << static_cast<int>(feedback.temperature_c) << '\n'
                  << std::boolalpha
                  << "enabled: " << feedback.enabled << '\n'
                  << "fault: " << feedback.fault << '\n';

        close(socket_fd);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}