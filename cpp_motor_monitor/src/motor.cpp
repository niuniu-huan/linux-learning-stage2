#include "motor.hpp"

#include <iomanip>
#include <iostream>

const char* motor_mode_to_string(MotorMode mode) noexcept {
    switch (mode) {
        case MotorMode::Disabled:
            return "disabled";
        case MotorMode::Enabled:
            return "enabled";
        case MotorMode::Fault:
            return "fault";
    }

    return "unknown";
}

Motor::Motor(int id, double max_temperature_c)
    : id_{id}, max_temperature_c_{max_temperature_c} {}

void Motor::update_feedback(double position_rad,
                            double velocity_rad_s,
                            double temperature_c) {
    position_rad_ = position_rad;
    velocity_rad_s_ = velocity_rad_s;
    temperature_c_ = temperature_c;

    if (temperature_c_ > max_temperature_c_) {
        mode_ = MotorMode::Fault;
    }
}

bool Motor::enable() {
    if (mode_ == MotorMode::Fault || temperature_c_ > max_temperature_c_) {
        mode_ = MotorMode::Fault;
        return false;
    }

    mode_ = MotorMode::Enabled;
    return true;
}

void Motor::disable() {
    if (mode_ != MotorMode::Fault) {
        mode_ = MotorMode::Disabled;
    }
}

void Motor::print_status() const {
    std::cout << std::fixed << std::setprecision(2)
              << "Motor " << id_
              << " | pos=" << position_rad_ << " rad"
              << " | vel=" << velocity_rad_s_ << " rad/s"
              << " | temp=" << temperature_c_ << " C"
              << " | mode=" << motor_mode_to_string(mode_)
              << '\n';
}