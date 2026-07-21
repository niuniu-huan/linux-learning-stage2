#include <iostream>

int main() {
    int* encoder_counts = new int[3]{100, 105, 110};

    // 故意错误：合法下标只有 0、1、2。
    std::cout << "Latest encoder count: " << encoder_counts[2] << '\n';

    delete[] encoder_counts;
    return 0;
}