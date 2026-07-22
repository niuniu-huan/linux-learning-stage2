#include "motor_command.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iomanip>
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
            .can_id = 0x200,
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

        const MotorCurrentCommand command{
            .target_current_raw = {1000, -500, 0, 2500},
        };
        const auto command_data = encode_motor_currents(command);

        struct can_frame tx_frame {};
        tx_frame.can_id = 0x200;
        tx_frame.can_dlc = command_data.size();
        std::copy(command_data.begin(), command_data.end(), tx_frame.data);

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
            throw std::runtime_error("timeout waiting for CAN command");
        }
        check(poll_result > 0, "poll");

        struct can_frame rx_frame {};
        check(read(socket_fd, &rx_frame, sizeof(rx_frame)) ==
                  static_cast<ssize_t>(sizeof(rx_frame)),
              "read CAN frame");

        std::cout << "Sent and received CAN ID: 0x"
                  << std::hex << (rx_frame.can_id & CAN_SFF_MASK)
                  << std::dec << '\n'
                  << "DLC: " << static_cast<int>(rx_frame.can_dlc)
                  << "\nData:";

        for (std::size_t i = 0; i < rx_frame.can_dlc; ++i) {
            std::cout << " 0x"
                      << std::uppercase << std::hex
                      << std::setw(2) << std::setfill('0')
                      << static_cast<unsigned int>(rx_frame.data[i]);
        }
        std::cout << '\n';

        close(socket_fd);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}