#include <cerrno>
#include <cstring>
#include <iostream>

#include <pty.h>
#include <termios.h>
#include <unistd.h>

bool configure_serial_port(int fd) {
    termios tty{};

    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "tcgetattr failed: " << std::strerror(errno) << '\n';
        return false;
    }

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag &= ~PARENB;             // 无校验位
    tty.c_cflag &= ~CSTOPB;             // 1 个停止位
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 // 8 个数据位
    tty.c_cflag |= CREAD | CLOCAL;      // 接收使能；忽略调制解调器控制线

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;  // 最多等待 1 秒

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "tcsetattr failed: " << std::strerror(errno) << '\n';
        return false;
    }

    return true;
}

int main() {
    int master_fd = -1;
    int slave_fd = -1;
    char slave_name[128]{};

    if (openpty(&master_fd, &slave_fd, slave_name, nullptr, nullptr) != 0) {
        std::cerr << "openpty failed: " << std::strerror(errno) << '\n';
        return 1;
    }

    if (!configure_serial_port(slave_fd)) {
        close(master_fd);
        close(slave_fd);
        return 1;
    }

    std::cout << "Configured pseudo serial port " << slave_name
              << " at 115200 8N1.\n";

    close(master_fd);
    close(slave_fd);
    return 0;
}