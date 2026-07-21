#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

class FileDescriptor {
public:
    explicit FileDescriptor(const char* path)
        : fd_{::open(path, O_RDONLY)} {
        if (fd_ < 0) {
            throw std::runtime_error(
                std::string{"open failed: "} + std::strerror(errno));
        }
    }

    ~FileDescriptor() {
        if (fd_ >= 0) {
            ::close(fd_);
        }
    }

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    int get() const noexcept {
        return fd_;
    }

private:
    int fd_;
};

int main() {
    try {
        FileDescriptor device{"/dev/null"};
        std::cout << "Opened /dev/null, fd=" << device.get() << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}