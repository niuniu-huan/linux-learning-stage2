#include "motor_command.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::chrono_literals;

volatile std::sig_atomic_t stop_requested = 0;

extern "C" void handle_stop_signal(int) {
    // 信号处理函数中只设置标志，不进行 write、cout 或 close。
    stop_requested = 1;
}

void check(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(
            std::string(message) + ": " + std::strerror(errno));
    }
}

bool send_command(int socket_fd, const MotorCurrentCommand& command) {
    const auto data = encode_motor_currents(command);

    struct can_frame frame {};
    frame.can_id = 0x200;
    frame.can_dlc = data.size();
    std::copy(data.begin(), data.end(), frame.data);

    const ssize_t written = write(socket_fd, &frame, sizeof(frame));
    if (written == static_cast<ssize_t>(sizeof(frame))) {
        return true;
    }

    if (written < 0 && errno == EINTR && stop_requested) {
        return false;
    }

    check(false, "write CAN frame");
    return false;
}

int main() {
    try {
        struct sigaction action {};
        action.sa_handler = handle_stop_signal;
        sigemptyset(&action.sa_mask);
        check(sigaction(SIGINT, &action, nullptr) == 0, "sigaction SIGINT");
        check(sigaction(SIGTERM, &action, nullptr) == 0, "sigaction SIGTERM");

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
        const MotorCommandSafety normal_safety{
            .output_enabled = true,
            .fault_active = false,
        };

        constexpr auto period = 10ms;
        auto next_send_time = std::chrono::steady_clock::now();
        int cycle = 0;

        std::cout << "Sending commands. Press Ctrl+C to stop safely.\n";

        while (!stop_requested) {
            const MotorCurrentCommand safe_command =
                make_safe_command(requested, normal_safety);

            if (!send_command(socket_fd, safe_command)) {
                break;
            }

            if (cycle % 10 == 0) {
                std::cout << "cycle " << cycle << ": normal command sent\n";
            }

            ++cycle;
            next_send_time += period;
            std::this_thread::sleep_until(next_send_time);
        }

        const MotorCurrentCommand zero_command{};
        send_command(socket_fd, zero_command);

        std::cout << "Stop requested: zero command sent, CAN socket closed.\n";
        close(socket_fd);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}