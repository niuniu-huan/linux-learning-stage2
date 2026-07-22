#include "motor_command.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <linux/can.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::chrono_literals;

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

        struct ifreq ifr {};
        std::strncpy(ifr.ifr_name, "vcan0", IFNAMSIZ - 1);
        check(ioctl(socket_fd, SIOCGIFINDEX, &ifr) >= 0, "ioctl SIOCGIFINDEX");

        struct sockaddr_can address {};
        address.can_family = AF_CAN;
        address.can_ifindex = ifr.ifr_ifindex;
        check(bind(socket_fd, reinterpret_cast<struct sockaddr*>(&address),
                   sizeof(address)) >= 0,
              "bind");

        const MotorCurrentCommand requested{
            .target_current_raw = {1000, -1000, 500, 0},
        };
        const MotorCommandSafety safety{
            .output_enabled = true,
            .fault_active = false,
        };

        constexpr auto period = 10ms;
        constexpr int send_count = 5;

        const auto start = std::chrono::steady_clock::now();
        auto next_send_time = start;

        for (int cycle = 0; cycle < send_count; ++cycle) {
            const MotorCurrentCommand safe_command =
                make_safe_command(requested, safety);
            const auto data = encode_motor_currents(safe_command);

            struct can_frame frame {};
            frame.can_id = 0x200;
            frame.can_dlc = data.size();
            std::copy(data.begin(), data.end(), frame.data);

            check(write(socket_fd, &frame, sizeof(frame)) ==
                      static_cast<ssize_t>(sizeof(frame)),
                  "write CAN frame");

            const auto elapsed = std::chrono::duration_cast<
                std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start);

            std::cout << "cycle " << cycle
                      << " sent at approximately "
                      << elapsed.count() << " ms\n";

            next_send_time += period;
            std::this_thread::sleep_until(next_send_time);
        }

        close(socket_fd);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}