#include "motor.hpp"

#include <iostream>

namespace {
int failures = 0;

void expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}
}  // namespace

int main() {
    Motor safe_motor{1, 80.0};
    safe_motor.update_feedback(0.0, 0.0, 40.0);
    expect(safe_motor.enable(), "safe motor should enable");

    Motor hot_motor{2, 80.0};
    hot_motor.update_feedback(0.0, 0.0, 85.0);
    expect(!hot_motor.enable(), "overheated motor should not enable");

    hot_motor.update_feedback(0.0, 0.0, 40.0);
    expect(!hot_motor.enable(), "fault state should remain latched");

    if (failures == 0) {
        std::cout << "All tests passed.\n";
    }

    return failures == 0 ? 0 : 1;
}