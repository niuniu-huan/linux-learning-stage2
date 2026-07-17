#include "motor.hpp"

#include <iostream>

int main() {
    Motor motor{2, 80.0};

    motor.update_feedback(0.50, 3.14, 40.0);
    motor.enable();
    motor.print_status();

    motor.update_feedback(0.55, 3.20, 85.0);
    motor.print_status();

    if (!motor.enable()) {
        std::cout << "Refusing to enable motor in fault state.\n";
    }

    return 0;
}