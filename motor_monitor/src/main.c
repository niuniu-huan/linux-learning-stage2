#include "motor.h"

int main(void) {
    MotorState motor = {
        .id = 1,
        .position_rad = 1.25,
        .velocity_rad_s = 6.28,
        .temperature_c = 36.8,
        .enabled = 1
    };

    motor_print_status(&motor);
    motor_set_enabled(&motor, 0);
    motor_print_status(&motor);

    return 0;
}