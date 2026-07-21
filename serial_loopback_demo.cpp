#include "motor_protocol.hpp"

#include <cerrno>
#include <cstring>
#include <iomanip>
#include <iostream>
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

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "tcsetattr failed: " << std::strerror(errno) << '\n';
        return false;
    }

    return true;
}

int main() {
    int controller_fd = -1;
    int host_fd = -1;
    char host_path[128]{};

    if (openpty(&controller_fd, &host_fd, host_path, nullptr, nullptr) != 0) {
        std::cerr << "openpty failed: " << std::strerror(errno) << '\n';
        return 1;
    }

    if (!configure_raw_serial(host_fd)) {
        close(controller_fd);
        close(host_fd);
        return 1;
    }

    const std::string frame = "MOTOR,1,1.25,6.28,36.8\n";

    if (write(controller_fd, frame.data(), frame.size()) < 0) {
        std::cerr << "write failed: " << std::strerror(errno) << '\n';
        close(controller_fd);
        close(host_fd);
        return 1;
    }

    char buffer[128]{};
    const ssize_t bytes_read = read(host_fd, buffer, sizeof(buffer));

    close(controller_fd);
    close(host_fd);

    if (bytes_read <= 0) {
        std::cerr << "read failed or timed out.\n";
        return 1;
    }

    std::string received(buffer, static_cast<std::size_t>(bytes_read));
    while (!received.empty() &&
           (received.back() == '\n' || received.back() == '\r')) {
        received.pop_back();
    }

    const auto feedback = parse_motor_feedback(received);
    if (!feedback) {
        std::cerr << "Invalid feedback frame: " << received << '\n';
        return 1;
    }

    std::cout << std::fixed << std::setprecision(2)
              << "Received from " << host_path
              << " | motor=" << feedback->id
              << " | pos=" << feedback->position_rad << " rad"
              << " | vel=" << feedback->velocity_rad_s << " rad/s"
              << " | temp=" << feedback->temperature_c << " C\n";

    return 0;
}