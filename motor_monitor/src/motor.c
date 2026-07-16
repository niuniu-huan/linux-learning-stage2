#include <stdio.h>
#include "motor.h"

void motor_print_status(const MotorState *motor) {
    printf("Motor %d | pos=%.2f rad | vel=%.2f rad/s | temp=%.1f C | %s\n",
           motor->id,
           motor->position_rad,
           motor->velocity_rad_s,
           motor->temperature_c,
           motor->enabled ? "enabled" : "disabled");
}

void motor_set_enabled(MotorState *motor, int enabled) {
    motor->enabled = enabled;
}