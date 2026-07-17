#include <stdio.h>

typedef struct {
    int id;
    double temperature_c;
} MotorState;

void motor_print_status(const MotorState *motor) {
    printf("Motor %d: %.1f C\n", motor->id, motor->temperature_c);
}

int main(void) {
    MotorState motor = {
        .id = 3,
        .temperature_c = 42.5
    };

    motor_print_status(&motor);

    return 0;
}