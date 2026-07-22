#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

int open_can_socket(const char* interface_name) {
    const int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        std::cerr << "socket failed: " << std::strerror(errno) << '\n';
        return -1;
    }

    ifreq ifr{};
    std::snprintf(ifr.ifr_name, IFNAMSIZ, "%s", interface_name);

    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "ioctl SIOCGIFINDEX failed: " << std::strerror(errno) << '\n';
        close(fd);
        return -1;
    }

    sockaddr_can address{};
    address.can_family = AF_CAN;
    address.can_ifindex = ifr.ifr_ifindex;

    if (bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "bind failed: " << std::strerror(errno) << '\n';
        close(fd);
        return -1;
    }

    return fd;
}

bool set_can_filter(int fd, canid_t wanted_id) {
    can_filter filter{};
    filter.can_id = wanted_id;
    filter.can_mask = CAN_SFF_MASK;

    if (setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER,
                   &filter, sizeof(filter)) != 0) {
        std::cerr << "setsockopt CAN_RAW_FILTER failed: "
                  << std::strerror(errno) << '\n';
        return false;
    }

    return true;
}

bool receive_frame(int fd, can_frame& frame, int timeout_ms) {
    pollfd event{};
    event.fd = fd;
    event.events = POLLIN;

    const int ready = poll(&event, 1, timeout_ms);
    if (ready == 0) {
        std::cerr << "receive timeout\n";
        return false;
    }

    if (ready < 0) {
        std::cerr << "poll failed: " << std::strerror(errno) << '\n';
        return false;
    }

    const ssize_t bytes = read(fd, &frame, sizeof(frame));
    if (bytes != static_cast<ssize_t>(sizeof(frame))) {
        std::cerr << "read failed: " << std::strerror(errno) << '\n';
        return false;
    }

    return true;
}

int main() {
    const int receiver_fd = open_can_socket("vcan0");
    const int sender_fd = open_can_socket("vcan0");

    if (receiver_fd < 0 || sender_fd < 0) {
        if (receiver_fd >= 0) {
            close(receiver_fd);
        }
        if (sender_fd >= 0) {
            close(sender_fd);
        }
        return 1;
    }

    if (!set_can_filter(receiver_fd, 0x201)) {
        close(receiver_fd);
        close(sender_fd);
        return 1;
    }

    can_frame unrelated{};
    unrelated.can_id = 0x123;
    unrelated.len = 1;
    unrelated.data[0] = 0xAA;

    can_frame motor_frame{};
    motor_frame.can_id = 0x201;
    motor_frame.len = 2;
    motor_frame.data[0] = 0x05;
    motor_frame.data[1] = 0xDC;

    if (write(sender_fd, &unrelated, sizeof(unrelated)) !=
            static_cast<ssize_t>(sizeof(unrelated)) ||
        write(sender_fd, &motor_frame, sizeof(motor_frame)) !=
            static_cast<ssize_t>(sizeof(motor_frame))) {
        std::cerr << "write failed: " << std::strerror(errno) << '\n';
        close(receiver_fd);
        close(sender_fd);
        return 1;
    }

    can_frame received{};
    const bool received_ok = receive_frame(receiver_fd, received, 1000);

    close(receiver_fd);
    close(sender_fd);

    if (!received_ok) {
        return 1;
    }

    std::cout << "Received CAN ID: 0x"
              << std::hex << std::uppercase << received.can_id << '\n';

    std::cout << "Data:";
    for (int i = 0; i < received.len; ++i) {
        std::cout << " 0x"
                  << std::setw(2) << std::setfill('0')
                  << static_cast<int>(received.data[i]);
    }
    std::cout << std::dec << '\n';

    return 0;
}