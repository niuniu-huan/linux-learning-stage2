#ifndef MOTOR_HPP
#define MOTOR_HPP

enum class MotorMode {
    Disabled,
    Enabled,
    Fault
};

const char* motor_mode_to_string(MotorMode mode) noexcept;

class Motor {
public:
    Motor(int id, double max_temperature_c);

    void update_feedback(double position_rad,
                         double velocity_rad_s,
                         double temperature_c);

    bool enable();
    void disable();
    void print_status() const;

private:
    int id_;
    double max_temperature_c_;
    double position_rad_{0.0};
    double velocity_rad_s_{0.0};
    double temperature_c_{0.0};
    MotorMode mode_{MotorMode::Disabled};
};

#endif