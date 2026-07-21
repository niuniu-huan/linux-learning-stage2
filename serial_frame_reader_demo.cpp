#include "motor_protocol.hpp"

#include <cerrno>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <optional>
#include <poll.h>
#include <pty.h>
#include <string>
#include <termios.h>
#include <unistd.h>

bool configure_raw_serial(int fd) {
    termios tty{};

    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "tcgetattr failed: " << std::strerror(errno) << '\n';
        return false;
    }

    cfmakeraw(&tty);
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    return tcsetattr(fd, TCSANOW, &tty) == 0;
}

std::optional<std::string> read_line(int fd, int timeout_ms) {
    std::string line;

    while (line.size() < 256) {
        pollfd event{};
        event.fd = fd;
        event.events = POLLIN;

        const int result = poll(&event, 1, timeout_ms);
        if (result == 0) {
            std::cerr << "Timed out while waiting for a serial frame.\n";
            return std::nullopt;
        }

        if (result < 0) {
            std::cerr << "poll failed: " << std::strerror(errno) << '\n';
            return std::nullopt;
        }

        char byte{};
        if (read(fd, &byte, 1) != 1) {
            std::cerr << "read failed: " << std::strerror(errno) << '\n';
            return std::nullopt;
        }

        if (byte == '\n') {
            return line;
        }

        if (byte != '\r') {
            line += byte;
        }
    }

    std::cerr << "Serial frame is too long.\n";
    return std::nullopt;
}

int main() {
    int controller_fd = -1;
    int host_fd = -1;
    char host_path[128]{};

    if (openpty(&controller_fd, &host_fd, host_path, nullptr, nullptr) != 0 ||
        !configure_raw_serial(host_fd)) {
        std::cerr << "Failed to create/configure pseudo serial port.\n";
        return 1;
    }

    const std::string first_chunk = "MOTOR,1,1.25,";
    const std::string second_chunk = "6.28,36.8\n";

    write(controller_fd, first_chunk.data(), first_chunk.size());
    write(controller_fd, second_chunk.data(), second_chunk.size());

    const auto line = read_line(host_fd, 1000);

    close(controller_fd);
    close(host_fd);

    if (!line) {
        return 1;
    }

    const auto feedback = parse_motor_feedback(*line);
    if (!feedback) {
        std::cerr << "Invalid feedback frame: " << *line << '\n';
        return 1;
    }

    std::cout << std::fixed << std::setprecision(2)
              << "Received complete frame from " << host_path
              << " | motor=" << feedback->id
              << " | pos=" << feedback->position_rad << " rad"
              << " | vel=" << feedback->velocity_rad_s << " rad/s"
              << " | temp=" << feedback->temperature_c << " C\n";

    return 0;
}