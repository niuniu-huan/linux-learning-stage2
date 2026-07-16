#include <stdio.h>

typedef struct {
    int id;
    float position_rad;
    float velocity_rad_s;
    float temperature_c;
    int enabled;
} MotorState;

int main(void) {
    MotorState motor = {
        .id = 1,
        .position_rad = 1.25f,
        .velocity_rad_s = 12.50f,
        .temperature_c = 36.80f,
        .enabled = 1
    };

    printf("Motor ID: %d\n", motor.id);
    printf("Position: %.2f rad\n", motor.position_rad);
    printf("Velocity: %.2f rad/s\n", motor.velocity_rad_s);
    printf("Temperature: %.2f C\n", motor.temperature_c);
    printf("Enabled: %s\n", motor.enabled ? "yes" : "no");

    return 0;
}