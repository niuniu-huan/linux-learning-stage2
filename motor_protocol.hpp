#ifndef MOTOR_PROTOCOL_HPP
#define MOTOR_PROTOCOL_HPP

#include <optional>
#include <string>

struct MotorFeedback {
    int id;
    double position_rad;
    double velocity_rad_s;
    double temperature_c;
};

std::optional<MotorFeedback> parse_motor_feedback(const std::string& frame);

#endif