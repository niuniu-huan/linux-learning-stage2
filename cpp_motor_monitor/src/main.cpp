#include "motor.hpp"

#include <iostream>

int main() {
    Motor motor{2, 80.0};

    motor.update_feedback(0.50, 3.14, 40.0);

    if (!motor.enable()) {
        std::cerr << "Motor temperature is too high.\n";
        return 1;
    }

    motor.print_status();
    motor.disable();
    motor.print_status();

    return 0;
}