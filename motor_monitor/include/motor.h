#ifndef MOTOR_H
#define MOTOR_H

typedef struct {
    int id;
    double position_rad;
    double velocity_rad_s;
    double temperature_c;
    int enabled;
} MotorState;

void motor_print_status(const MotorState *motor);
void motor_set_enabled(MotorState *motor, int enabled);

#endif