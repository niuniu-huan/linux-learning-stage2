#include <stdio.h>

int main(void) {
    int motor_id = 2;
    double speed_rpm = 60.0;
    const double pi = 3.141592653589793;
    double speed_rad_s = speed_rpm * 2.0 * pi / 60.0;

    printf("Motor ID: %d\n", motor_id);
    printf("Speed: %.2f rpm\n", speed_rpm);
    printf("Speed: %.3f rad/s\n", speed_rad_s);

    return 0;
}